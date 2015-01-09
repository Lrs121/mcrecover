/***************************************************************************
 * GameCube Memory Card Recovery Program.                                  *
 * IconAnimHelper.hpp: Icon animation helper.                              *
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

#ifndef __MCRECOVER_ICONANIMHELPER_HPP__
#define __MCRECOVER_ICONANIMHELPER_HPP__

// Qt includes.
#include <QtCore/QObject>
#include <QtGui/QPixmap>

class GcnFile;

class IconAnimHelperPrivate;

class IconAnimHelper : public QObject
{
	Q_OBJECT

	Q_PROPERTY(const GcnFile* file READ file WRITE setFile)
	Q_PROPERTY(bool animated READ isAnimated STORED false)
	Q_PROPERTY(QPixmap icon READ icon STORED false)

	public:
		IconAnimHelper();
		IconAnimHelper(const GcnFile *file);
		~IconAnimHelper();

	protected:
		IconAnimHelperPrivate *const d_ptr;
		Q_DECLARE_PRIVATE(IconAnimHelper)
	private:
		Q_DISABLE_COPY(IconAnimHelper)

	public:
		/**
		 * Time, in ms, for each frame for "fast" animated icons.
		 * TODO: Figure out the correct timer interval.
		 */
		static const int FAST_ANIM_TIMER = 125;

		/**
		 * Get the GcnFile this IconAnimHelper is handling.
		 * @return GcnFile.
		 */
		const GcnFile *file(void) const;

		/**
		 * Set the GcnFile this IconAnimHelper should handle.
		 * @param file GcnFile.
		 */
		void setFile(const GcnFile *file);

		/**
		 * Reset the animation state.
		 */
		void reset(void);

		/**
		 * Does this file have an animated icon?
		 * @return True if the icon is animated; false if not, or if no file is loaded.
		 */
		bool isAnimated(void) const;

		/**
		 * Get the current icon for this file.
		 * @return Current icon.
		 */
		QPixmap icon(void) const;

		/**
		 * Timer tick for the animation counter.
		 * @return True if the current icon has been changed; false if not.
		 */
		bool tick(void);

	protected slots:
		/**
		 * GcnFile object was destroyed.
		 * @param obj QObject that was destroyed.
		 */
		void file_destroyed_slot(QObject *obj = 0);
};

#endif /* __MCRECOVER_ICONANIMHELPER_HPP__ */
