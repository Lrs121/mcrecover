/***************************************************************************
 * GameCube Memory Card Recovery Program.                                  *
 * GcnCard.hpp: Memory Card reader class.                                  *
 *                                                                         *
 * Copyright (c) 2012-2015 by David Korth.                                 *
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

#ifndef __MCRECOVER_CARD_GCNCARD_HPP__
#define __MCRECOVER_CARD_GCNCARD_HPP__

// C includes.
#include <stdint.h>

// Qt includes and classes.
#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtCore/QLinkedList>
class QTextCodec;

#include "card.h"
#include "Checksum.hpp"
#include "SearchData.hpp"

// MemCardFile
class MemCardFile;

class GcnCardPrivate;
class GcnCard : public QObject
{
	Q_OBJECT

	Q_FLAGS(Error Errors)

	Q_PROPERTY(bool open READ isOpen)
	Q_PROPERTY(QString errorString READ errorString)
	Q_PROPERTY(QString filename READ filename)
	Q_PROPERTY(int filesize READ filesize);
	Q_PROPERTY(int sizeInBlocks READ sizeInBlocks)
	Q_PROPERTY(int sizeInBlocksNoSys READ sizeInBlocksNoSys)
	Q_PROPERTY(int freeBlocks READ freeBlocks)
	Q_PROPERTY(int blockSize READ blockSize)
	Q_PROPERTY(QString serialNumber READ serialNumber)
	Q_PROPERTY(int encoding READ encoding)
	Q_PROPERTY(int numFiles READ numFiles)
	Q_PROPERTY(bool empty READ isEmpty)
	// TODO: Register Checksum::ChecksumValue metatype?
	//Q_PROPERTY(Checksum::ChecksumValue headerChecksumValue READ headerChecksumValue)
	Q_PROPERTY(int activeDatIdx READ activeDatIdx WRITE setActiveDatIdx)
	Q_PROPERTY(int activeDatHdrIdx READ activeDatHdrIdx)
	Q_PROPERTY(int activeBatIdx READ activeBatIdx WRITE setActiveBatIdx)
	Q_PROPERTY(int activeBatHdrIdx READ activeBatHdrIdx)

	protected:
		GcnCard(QObject *parent = 0);
	public:
		~GcnCard();

	public:
		/**
		 * Open an existing Memory Card image.
		 * @param filename Filename.
		 * @param parent Parent object.
		 * @return GcnCard object, or nullptr on error.
		 */
		static GcnCard *open(const QString& filename, QObject *parent);

		/**
		 * Format a new Memory Card image.
		 * @param filename Filename.
		 * @param parent Parent object.
		 * @return GcnCard object, or nullptr on error.
		 */
		static GcnCard *format(const QString& filename, QObject *parent);

	protected:
		GcnCardPrivate *const d_ptr;
		Q_DECLARE_PRIVATE(GcnCard)
	private:
		Q_DISABLE_COPY(GcnCard)

	signals:
		/**
		 * Files are about to be added to the GcnCard.
		 * @param start First file index.
		 * @param end Last file index.
		 */
		void filesAboutToBeInserted(int start, int end);

		/**
		 * Files have been added to the GcnCard.
		 */
		void filesInserted(void);

		/**
		 * Files are about to be removed from the GcnCard.
		 * @param start First file index.
		 * @param end Last file index.
		 */
		void filesAboutToBeRemoved(int start, int end);

		/**
		 * Files have been removed from the GcnCard.
		 */
		void filesRemoved(void);

		/**
		 * Block count has changed.
		 * @param sizeInBlocksNoSys Size in blocks, minus the 5 reserved blocks.
		 * @param freeBlocks Number of free blocks.
		 */
		void blockCountChanged(int sizeInBlocksNoSys, int freeBlocks);

	public:
		/**
		 * Check if the memory card is open.
		 * @return True if open; false if not.
		 */
		bool isOpen(void) const;

		/**
		 * Get the last error string.
		 * Usually used for open() errors.
		 * TODO: Change to error code constants for translation?
		 * @return Error string.
		 */
		QString errorString(void) const;

		/**
		 * Get the memory card filename.
		 * @return Memory card filename, or empty string if not open.
		 */
		QString filename(void) const;

		/**
		 * Get the size of the memory card image, in bytes.
		 * This is the full size of the memory card image.
		 * @return Size of the memory card image, in bytes. (Negative on error)
		 */
		quint64 filesize(void) const;

		/**
		 * Get the size of the memory card, in blocks.
		 * NOTE: Includes the 5 reserved blocks. (e.g. MC1019 would return 1024)
		 * @return Size of the memory card, in blocks. (Negative on error)
		 */
		int sizeInBlocks(void) const;

		/**
		 * Get the size of the memory card, in blocks. [minus 5 reserved blocks]
		 * NOTE: Does NOT include the 5 reserved blocks. (e.g. MC1019 would return 1019)
		 * @return Size of the memory card, in blocks. (Negative on error)
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
		 * Get the memory card text encoding ID.
		 * @return 0 for ANSI (ISO-8859-1); 1 for SJIS; negative on error.
		 */
		int encoding(void) const;

		/**
		 * Get the text encoding ID for a given region.
		 * @param region Region code. (If 0, use the memory card's encoding.)
		 * @return Text encoding ID.
		 */
		int encodingForRegion(char region) const;

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
		 * @return MemCardFile added to the GcnCard, or nullptr on error.
		 */
		MemCardFile *addLostFile(const card_direntry *dirEntry);

		/**
		 * Add a "lost" file.
		 * @param dirEntry Directory entry.
		 * @param fatEntries FAT entries.
		 * @return MemCardFile added to the GcnCard, or nullptr on error.
		 */
		MemCardFile *addLostFile(const card_direntry *dirEntry, const QVector<uint16_t> &fatEntries);

		/**
		 * Add "lost" files.
		 * @param filesFoundList List of SearchData.
		 * @return List of MemCardFiles added to the GcnCard, or empty list on error.
		 */
		QList<MemCardFile*> addLostFiles(const QLinkedList<SearchData> &filesFoundList);

		/**
		 * Get the header checksum value.
		 * NOTE: Header checksum is always AddInvDual16.
		 * @return Header checksum value.
		 */
		Checksum::ChecksumValue headerChecksumValue(void) const;

		/**
		 * Get the active Directory Table index.
		 * @return Active Directory Table index. (0 or 1)
		 */
		int activeDatIdx(void) const;

		/**
		 * Set the active Directory Table index.
		 * NOTE: This function reloads the file list, without lost files.
		 * @param idx Active Directory Table index. (0 or 1)
		 */
		void setActiveDatIdx(int idx);

		/**
		 * Get the active Directory Table index according to the card header.
		 * @return Active Directory Table index (0 or 1), or -1 if both are invalid.
		 */
		int activeDatHdrIdx(void) const;

		/**
		 * Is a Directory Table valid?
		 * @param idx Directory Table index. (0 or 1)
		 * @return True if valid; false if not valid or idx is invalid.
		 */
		bool isDatValid(int idx) const;

		/**
		 * Get the active Block Table index.
		 * @return Active Block Table index. (0 or 1)
		 */
		int activeBatIdx(void) const;

		/**
		 * Set the active Block Table index.
		 * NOTE: This function reloads the file list, without lost files.
		 * @param idx Active Block Table index. (0 or 1)
		 */
		void setActiveBatIdx(int idx);

		/**
		 * Get the active Block Table index according to the card header.
		 * @return Active Block Table index (0 or 1), or -1 if both are invalid.
		 */
		int activeBatHdrIdx(void) const;

		/**
		 * Is a Block Table valid?
		 * @param idx Block Table index. (0 or 1)
		 * @return True if valid; false if not valid or idx is invalid.
		 */
		bool isBatValid(int idx) const;

		/**
		 * Memory card errors.
		 */
		enum Error {
			// Errors are ordered in order of severity.

			// Memory card is too small. (512 KB min)
			MCE_SZ_TOO_SMALL	= 0x01,
			// Memory card is too big. (16 MB max)
			MCE_SZ_TOO_BIG		= 0x02,
			// Memory card size is not a power of two.
			MCE_SZ_NON_POW2		= 0x04,

			// Header checksum is invalid.
			MCE_INVALID_HEADER	= 0x10,
			// Both DATs are invalid.
			MCE_INVALID_DATS	= 0x20,
			// Bot BATs are invalid.
			MCE_INVALID_BATS	= 0x40,

			// TODO: File open errors?
		};
		Q_DECLARE_FLAGS(Errors, Error)

		/**
		 * Have any errors been detected in this Memory Card?
		 * @return Error flags.
		 */
		QFlags<Error> errors(void) const;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GcnCard::Errors);

#endif /* __MCRECOVER_CARD_GCNCARD_HPP__ */
