//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of BrainCognize.
//
// BrainCognize is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BrainCognize is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BrainCognize. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef BMAINAPPLICATION_H
#define BMAINAPPLICATION_H

/**
 * Starts up the QT GUI.
 */
class BMainApplication
{
public:
    /**
     * QT GUI needs a QApplication instance with passed argc and argv.
     *
     * \return Exitcode of QT GUI.
     */
    int runQT( int argc, char* argv[] );

protected:
private:
};

#endif  // BMAINAPPLICATION_H

