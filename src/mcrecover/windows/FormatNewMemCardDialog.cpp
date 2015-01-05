/***************************************************************************
 * GameCube Memory Card Recovery Program.                                  *
 * FormatNewMemCardDialog.cpp: Format New Memory Card Image dialog.        *
 *                                                                         *
 * Copyright (c) 2015 by David Korth.                                      *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

#include "FormatNewMemCardDialog.hpp"

/** FormatNewMemCardDialogPrivate **/

#include "ui_FormatNewMemCardDialog.h"
class FormatNewMemCardDialogPrivate
{
	public:
		FormatNewMemCardDialogPrivate(FormatNewMemCardDialog *q);

	protected:
		FormatNewMemCardDialog *const q_ptr;
		Q_DECLARE_PUBLIC(FormatNewMemCardDialog)
	private:
		Q_DISABLE_COPY(FormatNewMemCardDialogPrivate)

	public:
		Ui::FormatNewMemCardDialog ui;

		/**
		 * Update the slider's size display.
		 * @param value Slider value.
		 */
		void updateSldSizeDisplay(int value);

		/**
		 * Update the slider's size display.
		 * Uses the current sliderPosition().
		 */
		void updateSldSizeDisplay(void);
};

FormatNewMemCardDialogPrivate::FormatNewMemCardDialogPrivate(FormatNewMemCardDialog* q)
	: q_ptr(q)
{ }

/**
 * Update the slider's size display.
 * @param value Slider value.
 */
void FormatNewMemCardDialogPrivate::updateSldSizeDisplay(int value)
{
	int sz_mbit = (4 << value);
	int sz_blocks = ((sz_mbit * 16) - 5);
	ui.lblSizeValue->setText(FormatNewMemCardDialog::tr("%Ln block(s)", "", sz_blocks));

	// Show a warning for >251 blocks.
	if (value > 2) {
		ui.lblSizeValue->setStyleSheet(QLatin1String(
			"QLabel { font-weight: bold; color: red; }"));
	} else {
		ui.lblSizeValue->setStyleSheet(QString());
	}
}

/**
 * Update the slider's size display.
 * Uses the current sliderPosition().
 */
void FormatNewMemCardDialogPrivate::updateSldSizeDisplay(void)
{
	updateSldSizeDisplay(ui.sldSize->sliderPosition());
}

/** FormatNewMemCardDialog **/

/**
 * Initialize the Format New Memory Card Image dialog.
 * @param parent Parent widget.
 */
FormatNewMemCardDialog::FormatNewMemCardDialog(QWidget *parent)
	: QDialog(parent,
		Qt::Dialog |
		Qt::CustomizeWindowHint |
		Qt::WindowTitleHint |
		Qt::WindowSystemMenuHint |
		Qt::WindowCloseButtonHint)
	, d_ptr(new FormatNewMemCardDialogPrivate(this))
{
	init();
}

/**
 * Common initialization function for all constructors.
 */
void FormatNewMemCardDialog::init(void)
{
	Q_D(FormatNewMemCardDialog);
	d->ui.setupUi(this);

	// Make sure the window is deleted on close.
	this->setAttribute(Qt::WA_DeleteOnClose, true);

#ifdef Q_OS_MAC
	// Remove the window icon. (Mac "proxy icon")
	this->setWindowIcon(QIcon());
#endif

	// Update the slider's size display.
	d->updateSldSizeDisplay();
}

/**
 * Shut down the Format New Memory Card Image dialog.
 */
FormatNewMemCardDialog::~FormatNewMemCardDialog()
{
	delete d_ptr;
}

/**
 * Widget state has changed.
 * @param event State change event.
 */
void FormatNewMemCardDialog::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		// Retranslate the UI.
		Q_D(FormatNewMemCardDialog);
		d->ui.retranslateUi(this);

		// Update the slider's size display.
		d->updateSldSizeDisplay();
	}

	// Pass the event to the base class.
	this->QDialog::changeEvent(event);
}

void FormatNewMemCardDialog::on_sldSize_sliderMoved(int value)
{
	Q_D(FormatNewMemCardDialog);
	d->updateSldSizeDisplay(value);
}

void FormatNewMemCardDialog::on_sldSize_valueChanged(int value)
{
	Q_D(FormatNewMemCardDialog);
	d->updateSldSizeDisplay(value);
}
