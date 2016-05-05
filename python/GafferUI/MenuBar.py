##########################################################################
#
#  Copyright (c) 2011-2012, John Haddon. All rights reserved.
#  Copyright (c) 2011-2012, Image Engine Design Inc. All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#
#      * Neither the name of John Haddon nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##########################################################################

import IECore

import Gaffer
import GafferUI

QtCore = GafferUI._qtImport( "QtCore" )
QtGui = GafferUI._qtImport( "QtGui" )

class MenuBar( GafferUI.Widget ) :

	def __init__( self, definition, **kw ) :

		menuBar = QtGui.QMenuBar()
		menuBar.setSizePolicy( QtGui.QSizePolicy( QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed ) )
		menuBar.setNativeMenuBar( False )

		GafferUI.Widget.__init__( self, menuBar, **kw )

		self.__shortcutEventFilter = None
		self.definition = definition

		self.__visibilityChangedConnection = self.visibilityChangedSignal().connect( Gaffer.WeakMethod( self.__visibilityChanged ) )
		self.__parentChangedConnection = self.parentChangedSignal().connect( Gaffer.WeakMethod( self.__parentChanged ) )
		self.__setupShortcutEventFilter()

	def __setattr__( self, key, value ) :

		self.__dict__[key] = value
		if key=="definition" :

			self._qtWidget().clear()
			self.__subMenus = []

			done = set()
			for path, item in self.definition.items() :

				pathComponents = path.strip( "/" ).split( "/" )
				name = pathComponents[0]
				if not name in done :

					if len( pathComponents ) > 1 :
						subMenuDefinition = self.definition.reRooted( "/" + name )
					else :
						subMenuDefinition = item.subMenu or IECore.MenuDefinition()

					menu = GafferUI.Menu( subMenuDefinition, _qtParent=self._qtWidget() )
					menu._qtWidget().setTitle( name )
					self._qtWidget().addMenu( menu._qtWidget() )
					self.__subMenus.append( menu )

				done.add( name )

	def __setupShortcutEventFilter( self ) :

		if self.__shortcutEventFilter is not None :
			return

		shortcutTarget = self.parent()
		if shortcutTarget is None :
			return

		if isinstance( shortcutTarget.parent(), GafferUI.Window ) :
			shortcutTarget = shortcutTarget.parent()

		self.__shortcutEventFilter = _ShortcutEventFilter( self._qtWidget() )
		shortcutTarget._qtWidget().installEventFilter( self.__shortcutEventFilter )

	def __visibilityChanged( self, widget ) :

		if self.visible() :
			self.__setupShortcutEventFilter()

	def __parentChanged( self, widget ) :

		if self.__shortcutEventFilter is not None :
			self.__shortcutEventFilter.setParent( None )
			self.__shortcutEventFilter = None
			self.__setupShortcutEventFilter()

# We use this event filter to detect keyboard shortcuts and
# trigger our menu items using them if necessary. We can't just let
# Qt handle the shortcuts because it would scope all the shortcuts to the
# toplevel window. This would not only prevent the effective use of
# multiple menubars within a layout, it also conflicts badly with
# native shortcuts when we're hosted inside an application like Maya.
# See Menu.__buildAction() where we limit the default scope for the
# shortcuts in order to defer to our own code here.
class _ShortcutEventFilter( QtCore.QObject ) :

	def __init__( self, parent ) :

		QtCore.QObject.__init__( self, parent )

		self.__shortcutAction = None

	def eventFilter( self, qObject, qEvent ) :

		# Qt bubbles up these shortcut override events from
		# the focussed widget to the top of the hierarchy,
		# at each level asking "Do you want to process this
		# keypress yourself before I use it as a shortcut at
		# some other scope?". This gives us the opportunity
		# to do sane scoped handling of shortcuts, rather than
		# the everything-fighting-at-the-window-level carnage
		# that Qt has gone out of its way to create. We're
		# striving to do all event handling in GafferUI with this
		# bubble-up-until-handled methodology anyway, so doing
		# shortcuts this way seems to make sense.
		if qEvent.type() == qEvent.ShortcutOverride :

			self.__shortcutAction = None
			keySequence = self.__keySequence( qEvent )
			menuBar = GafferUI.Widget._owner( self.parent() )
			for menu in menuBar._MenuBar__subMenus :
				if menu._qtWidget().isEmpty() :
					menu._buildFully()
				action = self.__matchingAction( keySequence, menu._qtWidget() )
				if action is not None :
					# Store for use in KeyPress
					self.__shortcutAction = action
					qEvent.accept()
					return True

		# We handle the shortcut override event, but that just
		# means that we then have the option of handling the
		# associated keypress, which is what we do here.
		elif qEvent.type() == qEvent.KeyPress :

			if self.__shortcutAction is not None and self.__keySequence( qEvent ) in self.__shortcutAction.shortcuts() :
				self.__shortcutAction.trigger()
				self.__shortcutAction = None
				qEvent.accept()
				return True

		return QtCore.QObject.eventFilter( self, qObject, qEvent )

	def __keySequence( self, keyEvent ) :

		return QtGui.QKeySequence( keyEvent.key() | int( keyEvent.modifiers() ) )

	def __matchingAction( self, keySequence, menu ) :

		for action in menu.actions() :
			if keySequence in action.shortcuts() :
				return action
			if action.menu() is not None :
				a = self.__matchingAction( keySequence, action.menu() )
				if a is not None :
					return a

		return None
