/*
 * QwiicKeypad.cpp
 *
 *  Created on: Apr. 4, 2020
 *      Author: rir
 */

#include "QwiicKeypad.h"

constexpr uint8_t Address = 0x4b;
constexpr uint8_t Button = 0x03;
constexpr uint8_t Update = 0x06;

constexpr auto SRC_CLK = 12000000;

std::map<I2C_Type*, I2C::QwiicKeypad*> I2C::QwiicKeypad::instances_ = {
	std::make_pair(I2C0, nullptr),
	std::make_pair(I2C1, nullptr)
};

namespace I2C
{
	QwiicKeypad* QwiicKeypad::GetInstance(I2C_Type* channel)
	{
		assert(channel != nullptr);

		if (instances_[channel] == nullptr)
		{
			instances_[channel] = new QwiicKeypad(channel);
		}

		return instances_[channel];
	}

	status_t QwiicKeypad::Poll() const
	{
		// write update reg + va;
		uint8_t updateReg[] = { Update, 0x01 };
		const auto res1 = Write(updateReg);
		if (res1 != kStatus_Success) return res1;

		// write button reg
		uint8_t buttonReg[] = { Button };
		const auto res2 = Write(buttonReg);
		if (res2 != kStatus_Success) return res2;

		// read button
		char button = '';
		uint8_t res3 = ReadButton(&button);
		if (res3 != kStatus_Success) return res3;

		// hard delay
		for (int i = 0; i < 10000; ++i)
		{
			__asm volatile("nop");
		}

		if (button == -1)
		{
			return kStatus_I2C_UnexpectedState;
		}

		if (button == 0) return kStatus_Success;

		for (auto&& listener : listeners_)
		{
			listener(static_cast<uint8_t>(button) & 0xFF);
		}

		return kStatus_Success;
	}

	void QwiicKeypad::AddListener(const ButtonCallback onButtonPress)
	{
		listeners_.push_back(onButtonPress);
	}

	QwiicKeypad::QwiicKeypad(I2C_Type* channel) : channel_(channel)
	{
		i2c_master_config_t config{};
		I2C_MasterGetDefaultConfig(&config);
		I2C_MasterInit(channel, &config, SRC_CLK);
	}

	status_t QwiicKeypad::Write(uint8_t* regs) const
	{
		I2C_MasterStart(channel_, Address, kI2C_Write);

		volatile uint8_t status;

		while (!((status = I2C_GetStatusFlags(channel_)) & kI2C_MasterPendingFlag))
		{
		}

		const auto didWrite = I2C_MasterWriteBlocking(
			channel_,
			regs,
			sizeof regs,
			kI2C_TransferDefaultFlag
		);

		if (didWrite != kStatus_Success)
		{
			return didWrite;
		}

		I2C_MasterStop(channel_);

		return kStatus_Success;
	}

	status_t QwiicKeypad::ReadButton(char* out) const
	{
		I2C_MasterStart(channel_, Address, i2c_direction_t::kI2C_Read);

		volatile uint8_t status;
		while (!((status = I2C_GetStatusFlags(channel_)) & kI2C_MasterPendingFlag))
		{
		}

		uint8_t rx[] = { 0x00 };

		const auto didRead = I2C_MasterReadBlocking(channel_, rx, sizeof rx, kI2C_TransferDefaultFlag);

		if (didRead != kStatus_Success)
		{
			return didRead;
		}

		I2C_MasterStop(channel_);

		*out = rx[0];

		return kStatus_Success;
	}
} /* namespace I2C */
