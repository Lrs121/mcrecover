/***************************************************************************
 * GameCube Memory Card Recovery Program.                                  *
 * MemCardView.hpp: MemCard view widget.                                   *
 *                                                                         *
 * Copyright (c) 2012-2013 by David Korth.                                 *
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

#include "MemCardView.hpp"

#include "MemCard.hpp"
#include "Checksum.hpp"
#include "McRecoverQApplication.hpp"

// C includes.
#include <stdlib.h>


/** MemCardViewPrivate **/

class MemCardViewPrivate
{
	public:
		MemCardViewPrivate(MemCardView *q);
		~MemCardViewPrivate();

	private:
		MemCardView *const q;
		Q_DISABLE_COPY(MemCardViewPrivate);

	public:
		const MemCard *card;

		/**
		 * Update the widget display.
		 */
		void updateWidgetDisplay(void);
};

MemCardViewPrivate::MemCardViewPrivate(MemCardView *q)
	: q(q)
	, card(nullptr)
{ }

MemCardViewPrivate::~MemCardViewPrivate()
{ }


/**
 * Update the widget display.
 */
void MemCardViewPrivate::updateWidgetDisplay(void)
{
	if (!card) {
		// Hide the widget display.
		// TODO: Better method?
		q->lblBlockCount->setVisible(false);
		q->lblStatusIcon->setVisible(false);
		q->lblEncodingTitle->setVisible(false);
		q->lblEncoding->setVisible(false);
		q->lblChecksumActualTitle->setVisible(false);
		q->lblChecksumActual->setVisible(false);
		q->lblChecksumExpectedTitle->setVisible(false);
		q->lblChecksumExpected->setVisible(false);
		return;
	}

	// Show the widget display.
	q->lblBlockCount->setVisible(true);
	q->lblEncodingTitle->setVisible(true);
	q->lblEncoding->setVisible(true);
	q->lblChecksumActualTitle->setVisible(true);
	q->lblChecksumActual->setVisible(true);

	// Update the widget display.
	bool isCardHeaderValid = true;

	// Format the header checksum.
	QVector<Checksum::ChecksumValue> checksumValues;
	checksumValues.append(card->headerChecksumValue());
	QVector<QString> checksumValuesFormatted = Checksum::ChecksumValuesFormatted(checksumValues);
	if (checksumValuesFormatted.size() < 1) {
		// No checksum...
		q->lblChecksumActual->setText(q->tr("Unknown", "checksum"));
		q->lblChecksumExpectedTitle->setVisible(false);
		q->lblChecksumExpected->setVisible(false);
	} else {
		// Set the actual checksum text.
		q->lblChecksumActual->setText(checksumValuesFormatted.at(0));

		if (checksumValuesFormatted.size() > 1) {
			// At least one checksum is invalid.
			isCardHeaderValid = false;
			// Show the expected checksum.
			q->lblChecksumExpectedTitle->setVisible(true);
			q->lblChecksumExpected->setVisible(true);
			q->lblChecksumExpected->setText(checksumValuesFormatted.at(1));
		} else {
			// Checksums are all valid.
			// Hide the expected checksum.
			q->lblChecksumExpectedTitle->setVisible(false);
			q->lblChecksumExpected->setVisible(false);
			q->lblChecksumExpected->clear();
		}
	}

	// Validate some other aspects of the card header.
	if (isCardHeaderValid) {
		if (card->freeBlocks() > card->sizeInBlocksNoSys()) {
			// Free blocks count is wrong.
			isCardHeaderValid = false;
		}
		// TODO: Other aspects.
	}

	// Block count.
	q->lblBlockCount->setText(q->tr("%L1 block(s) (%L2 free)")
				.arg(card->sizeInBlocksNoSys())
				.arg(card->freeBlocks()));

	// Status icon.
	if (isCardHeaderValid) {
		// Card header is valid.
		q->lblStatusIcon->setVisible(false);
	} else {
		// Card header is invalid.
		QIcon icon = McRecoverQApplication::IconFromTheme(QLatin1String("dialog-error"));
		// TODO: What size?
		q->lblStatusIcon->setPixmap(icon.pixmap(16, 16));
		q->lblStatusIcon->setToolTip(q->tr("Memory card header is corrupted."));
		q->lblStatusIcon->setVisible(true);
	}

	// Encoding.
	QString encoding;
	switch (card->encoding()) {
		case SYS_FONT_ENCODING_SJIS:
			encoding = QLatin1String("Shift-JIS");
			break;

		case SYS_FONT_ENCODING_ANSI:
		default:
			encoding = QLatin1String("cp1252");
			break;
	}
	q->lblEncoding->setText(encoding);
}


/** McRecoverWindow **/

MemCardView::MemCardView(QWidget *parent)
	: QWidget(parent)
	, d(new MemCardViewPrivate(this))
{
	setupUi(this);

	// Set monospace fonts.
	QFont fntMonospace;
	fntMonospace.setFamily(QLatin1String("Monospace"));
	fntMonospace.setStyleHint(QFont::TypeWriter);
	fntMonospace.setBold(true);
	lblChecksumActual->setFont(fntMonospace);
	lblChecksumExpected->setFont(fntMonospace);

	d->updateWidgetDisplay();
}

MemCardView::~MemCardView()
{
	delete d;
}


/**
 * Get the MemCard being displayed.
 * @return MemCard.
 */
const MemCard *MemCardView::card(void) const
	{ return d->card; }

/**
 * Set the MemCard being displayed.
 * @param card MemCard.
 */
void MemCardView::setCard(const MemCard *card)
{
	// Disconnect the MemCard's destroyed() signal if a MemCard is already set.
	if (d->card) {
		disconnect(d->card, SIGNAL(destroyed(QObject*)),
			   this, SLOT(memCard_destroyed_slot(QObject*)));
	}

	d->card = card;

	// Connect the MemCard's destroyed() signal.
	if (d->card) {
		connect(d->card, SIGNAL(destroyed(QObject*)),
			this, SLOT(memCard_destroyed_slot(QObject*)));
	}

	// Update the widget display.
	d->updateWidgetDisplay();
}


/**
 * Widget state has changed.
 * @param event State change event.
 */
void MemCardView::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		// Retranslate the UI.
		retranslateUi(this);
		d->updateWidgetDisplay();
	}

	// Pass the event to the base class.
	this->QWidget::changeEvent(event);
}


/** Slots. **/


/**
 * MemCard object was destroyed.
 * @param obj QObject that was destroyed.
 */
void MemCardView::memCard_destroyed_slot(QObject *obj)
{
	if (obj == d->card) {
		// Our MemCard was destroyed.
		d->card = nullptr;

		// Update the widget display.
		d->updateWidgetDisplay();
	}
}
