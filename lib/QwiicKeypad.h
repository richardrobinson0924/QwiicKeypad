/*
 * QwiicKeypad.h
 *
 *  Created on: Apr. 4, 2020
 *      Author: rir
 */

#ifndef QWIICKEYPAD_H_
#define QWIICKEYPAD_H_

#include "../drivers/fsl_i2c.h"
#include "../device/LPC804.h"

#include <map>
#include <list>

namespace I2C
{
	/**
	 * A class with Singleton interfaces to communicate between the SparkFun Qwiic I2C Keypad
	 * and the LPC 804 development board.
	 */
	class QwiicKeypad
	{
	public:
		using ButtonCallback = void (*)(uint8_t);

		/**
		 * \brief Returns the Singleton instance associated with the specified I2C channel.
		 * \note The I2C interface for \c channel must have already been initialized
		 *
		 * \param channel one of the two I2C channels, \c I2C0 or \c I2C1
		 * \return the Singleton \c I2C::QwiicKeypad instance associated with the specified I2C channel
		 */
		static QwiicKeypad* GetInstance(I2C_Type* channel);

		/**
		 * \brief Polls the I2C Keypad once with a short delay post execution. If a button has been found to
		 * have been pressed since the last invocation of \c I2C::Keypad::Poll(), all registered
		 * callback functions will be invoked with the pressed button index as their parameter.
		 *
		 * \see I2C::Keypad::AddListener(ButtonCallback)
		 * \return \c kStatus_Success if the poll was successful;
		 *	<p> \c kStatus_I2C_UnexpectedState if the button cannot be read;
		 *	<p> another error status if there was an internal I2C error
		 */
		status_t Poll() const;

		/**
		 * \brief Registers the specified callback function to be invoked whenever a button is pressed.
		 * The parameter passed to the callback function is the index of the pressed button.
		 *
		 * \param onButtonPress a function accepting a single \c uint8_t parameter
		 */
		void AddListener(ButtonCallback onButtonPress);

	private:
		static std::map<I2C_Type*, QwiicKeypad*> instances_;

		std::list<ButtonCallback> listeners_ = {};
		const I2C_Type* channel_;

		explicit QwiicKeypad(I2C_Type* channel);

		status_t Write(uint8_t* regs) const;
		status_t ReadButton(char* out) const;
	};
} /* namespace I2C */

#endif /* QWIICKEYPAD_H_ */
