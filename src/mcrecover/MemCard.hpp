/***************************************************************************
 * GameCube Memory Card Recovery Program.                                  *
 * MemCard.hpp: Memory Card reader class.                                  *
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

#ifndef __MCRECOVER_MEMCARD_HPP__
#define __MCRECOVER_MEMCARD_HPP__

// C includes.
#include <stdint.h>

// Qt includes and classes.
#include <QtCore/QObject>
#include <QtCore/QVector>
class QTextCodec;

#include "card.h"
#include "Checksum.hpp"

// MemCardFile
class MemCardFile;

// MemCard private class.
class MemCardPrivate;

class MemCard : public QObject
{
	Q_OBJECT
	
	public:
		MemCard(const QString& filename, QObject *parent = 0);
		~MemCard();

	private:
		friend class MemCardPrivate;
		MemCardPrivate *const d;
		Q_DISABLE_COPY(MemCard);

	signals:
		/**
		 * Files are about to be added to the MemCard.
		 * @param start First file index.
		 * @param end Last file index.
		 */
		void filesAboutToBeInserted(int start, int end);

		/**
		 * Files have been added to the MemCard.
		 */
		void filesInserted(void);

		/**
		 * Files are about to be removed from the MemCard.
		 * @param start First file index.
		 * @param end Last file index.
		 */
		void filesAboutToBeRemoved(int start, int end);

		/**
		 * Files have been removed from the MemCard.
		 */
		void filesRemoved(void);

	public:
		/**
		 * Check if the memory card is open.
		 * @return True if open; false if not.
		 */
		bool isOpen(void) const;

		/**
		 * Get the memory card filename.
		 * @return Memory card filename, or empty string if not open.
		 */
		QString filename(void) const;

		/**
		 * Get the size of the memory card, in blocks.
		 * NOTE: Includes the 5 reserved blocks. (e.g. MC1019 would return 1024)
		 * @return Size of memory card, in blocks. (Negative on error)
		 */
		int sizeInBlocks(void) const;

		/**
		 * Get the size of the memory card, in blocks. [minus 5 reserved blocks]
		 * NOTE: Does NOT include the 5 reserved blocks. (e.g. MC1019 would return 1019)
		 * @return Size of memory card, in blocks. (Negative on error)
		 */
		int sizeInBlocksNoSys(void) const;

		/**
		 * Get the number of free blocks.
		 * @return Free blocks. (Negative on error)
		 */
		int freeBlocks(void) const;

		/**
		 * Get the memory card block size, in bytes.
		 * @return Memory card block size, in bytes. (Negative on error)
		 */
		int blockSize(void) const;

		/**
		 * Read a block.
		 * @param buf Buffer to read the block data into.
		 * @param siz Size of buffer.
		 * @param blockIdx Block index.
		 * @return Bytes read on success; negative on error.
		 */
		int readBlock(void *buf, int siz, uint16_t blockIdx);

		/**
		 * Get the memory card's serial number.
		 * @return Memory card's serial number.
		 */
		QString serialNumber(void) const;

		/**
		 * Get the memory card encoding.
		 * @return 0 for ANSI (ISO-8859-1); 1 for SJIS; negative on error.
		 */
		int encoding(void) const;

		/**
		 * Get the QTextCodec for a given region.
		 * @param region Region code. (If 0, use the memory card's encoding.)
		 * @return QTextCodec.
		 */
		QTextCodec *textCodec(char region = 0) const;

		/**
		 * Get the number of files in the file table.
		 * @return Number of files, or negative on error.
		 */
		int numFiles(void) const;

		/**
		 * Is the card empty?
		 * @return True if empty; false if not.
		 */
		bool isEmpty(void) const;

		/**
		 * Get a MemCardFile object.
		 * @param idx File number.
		 * @return MemCardFile object, or nullptr on error.
		 */
		MemCardFile *getFile(int idx);

		/**
		 * Remove all "lost" files.
		 */
		void removeLostFiles(void);

		/**
		 * Get the used block map.
		 * NOTE: This is only valid for regular files, not "lost" files.
		 * @return Used block map.
		 */
		QVector<uint8_t> usedBlockMap(void);

		/**
		 * Add a "lost" file.
		 * NOTE: This is a debugging version.
		 * Add more comprehensive versions with a block map specification.
		 * @return MemCardFile added to the MemCard, or nullptr on error.
		 */
		MemCardFile *addLostFile(const card_direntry *dirEntry);

		/**
		 * Add a "lost" file.
		 * @param dirEntry Directory entry.
		 * @param fatEntries FAT entries.
		 * @return MemCardFile added to the MemCard, or nullptr on error.
		 */
		MemCardFile *addLostFile(const card_direntry *dirEntry, QVector<uint16_t> fatEntries);

		/**
		 * Get the header checksum value.
		 * NOTE: Header checksum is always AddInvDual16.
		 * @return Header checksum value.
		 */
		Checksum::ChecksumValue headerChecksumValue(void) const;
};

#endif /* __MCRECOVER_MEMCARD_HPP__ */
