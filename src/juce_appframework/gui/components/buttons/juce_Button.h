/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef __JUCE_BUTTON_JUCEHEADER__
#define __JUCE_BUTTON_JUCEHEADER__

#include "../juce_Component.h"
#include "../keyboard/juce_KeyListener.h"
#include "../../../application/juce_ApplicationCommandManager.h"
#include "../../../../juce_core/containers/juce_SortedSet.h"
#include "../windows/juce_TooltipWindow.h"
#include "../../../events/juce_Timer.h"
class Button;


//==============================================================================
/**
    Used to receive callbacks when a button is clicked.

    @see Button::addButtonListener, Button::removeButtonListener
*/
class JUCE_API  ButtonListener
{
public:
    /** Destructor. */
    virtual ~ButtonListener()                               {}

    /** Called when the button is clicked. */
    virtual void buttonClicked (Button* button) = 0;

    /** Called when the button's state changes. */
    virtual void buttonStateChanged (Button*)               {}
};


//==============================================================================
/**
    A base class for buttons.

    This contains all the logic for button behaviours such as enabling/disabling,
    responding to shortcut keystrokes, auto-repeating when held down, toggle-buttons
    and radio groups, etc.

    @see TextButton, DrawableButton, ToggleButton
*/
class JUCE_API  Button  : public Component,
                          public SettableTooltipClient,
                          public ApplicationCommandManagerListener,
                          private KeyListener
{
protected:
    //==============================================================================
    /** Creates a button.

        @param buttonName           the text to put in the button (the component's name is also
                                    initially set to this string, but these can be changed later
                                    using the setName() and setButtonText() methods)
    */
    Button (const String& buttonName);

public:
    /** Destructor. */
    virtual ~Button();

    //==============================================================================
    /** Changes the button's text.

        @see getButtonText
    */
    void setButtonText (const String& newText) throw();

    /** Returns the text displayed in the button.

        @see setButtonText
    */
    const String getButtonText() const throw()                  { return text; }

    //==============================================================================
    /** Returns true if the button is currently being held down by the mouse.

        @see isOver
    */
    bool isDown() const throw();

    /** Returns true if the mouse is currently over the button.

        This will be also be true if the mouse is being held down.

        @see isDown
    */
    bool isOver() const throw();

    //==============================================================================
    /** A button has an on/off state associated with it, and this changes that.

        By default buttons are 'off' and for simple buttons that you click to perform
        an action you won't change this. Toggle buttons, however will want to
        change their state when turned on or off.

        @param shouldBeOn               whether to set the button's toggle state to be on or
                                        off. If it's a member of a button group, this will
                                        always try to turn it on, and to turn off any other
                                        buttons in the group
        @param sendChangeNotification   if true, a callback will be made to clicked(); if false
                                        the button will be repainted but no notification will
                                        be sent
        @see getToggleState, setRadioGroupId
    */
    void setToggleState (const bool shouldBeOn,
                         const bool sendChangeNotification);

    /** Returns true if the button in 'on'.

        By default buttons are 'off' and for simple buttons that you click to perform
        an action you won't change this. Toggle buttons, however will want to
        change their state when turned on or off.

        @see setToggleState
    */
    bool getToggleState() const throw()                         { return isOn; }

    /** This tells the button to automatically flip the toggle state when
        the button is clicked.

        If set to true, then before the clicked() callback occurs, the toggle-state
        of the button is flipped.
    */
    void setClickingTogglesState (const bool shouldToggle) throw();

    /** Returns true if this button is set to be an automatic toggle-button.

        This returns the last value that was passed to setClickingTogglesState().
    */
    bool getClickingTogglesState() const throw();

    //==============================================================================
    /** Enables the button to act as a member of a mutually-exclusive group
        of 'radio buttons'.

        If the group ID is set to a non-zero number, then this button will
        act as part of a group of buttons with the same ID, only one of
        which can be 'on' at the same time. Note that when it's part of
        a group, clicking a toggle-button that's 'on' won't turn it off.

        To find other buttons with the same ID, this button will search through
        its sibling components for ToggleButtons, so all the buttons for a
        particular group must be placed inside the same parent component.

        Set the group ID back to zero if you want it to act as a normal toggle
        button again.

        @see getRadioGroupId
    */
    void setRadioGroupId (const int newGroupId);

    /** Returns the ID of the group to which this button belongs.

        (See setRadioGroupId() for an explanation of this).
    */
    int getRadioGroupId() const throw()                         { return radioGroupId; }

    //==============================================================================
    /** Registers a listener to receive events when this button's state changes.

        If the listener is already registered, this will not register it again.

        @see removeButtonListener
    */
    void addButtonListener (ButtonListener* const newListener) throw();

    /** Removes a previously-registered button listener

        @see addButtonListener
    */
    void removeButtonListener (ButtonListener* const listener) throw();

    //==============================================================================
    /** Causes the button to act as if it's been clicked.

        This will asynchronously make the button draw itself going down and up, and
        will then call back the clicked() method as if mouse was clicked on it.

        @see clicked
    */
    virtual void triggerClick();

    //==============================================================================
    /** Sets a command ID for this button to automatically invoke when it's clicked.

        When the button is pressed, it will use the given manager to trigger the
        command ID.

        Obviously be careful that the ApplicationCommandManager doesn't get deleted
        before this button is. To disable the command triggering, call this method and
        pass 0 for the parameters.

        If generateTooltip is true, then the button's tooltip will be automatically
        generated based on the name of this command and its current shortcut key.

        @see addShortcut, getCommandID
    */
    void setCommandToTrigger (ApplicationCommandManager* commandManagerToUse,
                              const int commandID,
                              const bool generateTooltip);

    /** Returns the command ID that was set by setCommandToTrigger().
    */
    int getCommandID() const throw()                { return commandID; }

    //==============================================================================
    /** Assigns a shortcut key to trigger the button.

        The button registers itself with its top-level parent component for keypresses.

        Note that a different way of linking buttons to keypresses is by using the
        setKeyPressToTrigger() method to invoke a command - the difference being that
        setting a shortcut allows the button to be temporarily linked to a keypress
        only while it's on-screen.

        @see clearShortcuts
    */
    void addShortcut (const KeyPress& key);

    /** Removes all key shortcuts that had been set for this button.

        @see addShortcut
    */
    void clearShortcuts();

    /** Returns true if the given keypress is a shortcut for this button.

        @see addShortcut
    */
    bool isRegisteredForShortcut (const KeyPress& key) const throw();

    //==============================================================================
    /** Sets an auto-repeat speed for the button when it is held down.

        (Auto-repeat is disabled by default).

        @param initialDelayInMillisecs      how long to wait after the mouse is pressed before
                                            triggering the next click. If this is zero, auto-repeat
                                            is disabled
        @param repeatDelayInMillisecs       the frequently subsequent repeated clicks should be
                                            triggered
        @param minimumDelayInMillisecs      if this is greater than 0, the auto-repeat speed will
                                            get faster, the longer the button is held down, up to the
                                            minimum interval specified here
    */
    void setRepeatSpeed (const int initialDelayInMillisecs,
                         const int repeatDelayInMillisecs,
                         const int minimumDelayInMillisecs = -1) throw();

    /** Sets whether the button click should happen when the mouse is pressed or released.

        By default the button is only considered to have been clicked when the mouse is
        released, but setting this to true will make it call the clicked() method as soon
        as the button is pressed.

        This is useful if the button is being used to show a pop-up menu, as it allows
        the click to be used as a drag onto the menu.
    */
    void setTriggeredOnMouseDown (const bool isTriggeredOnMouseDown) throw();

    /** Returns the number of milliseconds since the last time the button
        went into the 'down' state.
    */
    uint32 getMillisecondsSinceButtonDown() const throw();

    //==============================================================================
    /** (overridden from Component to do special stuff). */
    void setVisible (bool shouldBeVisible);

    //==============================================================================
    /** Sets the tooltip for this button.

        @see TooltipClient, TooltipWindow
    */
    void setTooltip (const String& newTooltip);

    // (implementation of the TooltipClient method)
    const String getTooltip();


    //==============================================================================
    /** A combination of these flags are used by setConnectedEdges().
    */
    enum ConnectedEdgeFlags
    {
        ConnectedOnLeft = 1,
        ConnectedOnRight = 2,
        ConnectedOnTop = 4,
        ConnectedOnBottom = 8
    };

    /** Hints about which edges of the button might be connected to adjoining buttons.

        The value passed in is a bitwise combination of any of the values in the
        ConnectedEdgeFlags enum.

        E.g. if you are placing two buttons adjacent to each other, you could use this to
        indicate which edges are touching, and the LookAndFeel might choose to drawn them
        without rounded corners on the edges that connect. It's only a hint, so the
        LookAndFeel can choose to ignore it if it's not relevent for this type of
        button.
    */
    void setConnectedEdges (const int connectedEdgeFlags) throw();

    /** Returns the set of flags passed into setConnectedEdges(). */
    int getConnectedEdgeFlags() const throw()                   { return connectedEdgeFlags; }

    /** Indicates whether the button adjoins another one on its left edge.
        @see setConnectedEdges
    */
    bool isConnectedOnLeft() const throw()                      { return (connectedEdgeFlags & ConnectedOnLeft) != 0; }

    /** Indicates whether the button adjoins another one on its right edge.
        @see setConnectedEdges
    */
    bool isConnectedOnRight() const throw()                     { return (connectedEdgeFlags & ConnectedOnRight) != 0; }

    /** Indicates whether the button adjoins another one on its top edge.
        @see setConnectedEdges
    */
    bool isConnectedOnTop() const throw()                       { return (connectedEdgeFlags & ConnectedOnTop) != 0; }

    /** Indicates whether the button adjoins another one on its bottom edge.
        @see setConnectedEdges
    */
    bool isConnectedOnBottom() const throw()                    { return (connectedEdgeFlags & ConnectedOnBottom) != 0; }


    //==============================================================================
    /** Used by setState(). */
    enum ButtonState
    {
        buttonNormal,
        buttonOver,
        buttonDown
    };

    /** Can be used to force the button into a particular state.

        This only changes the button's appearance, it won't trigger a click, or stop any mouse-clicks
        from happening.

        The state that you set here will only last until it is automatically changed when the mouse
        enters or exits the button, or the mouse-button is pressed or released.
    */
    void setState (const ButtonState newState);

    //==============================================================================
    juce_UseDebuggingNewOperator

protected:
    //==============================================================================
    /** This method is called when the button has been clicked.

        Subclasses can override this to perform whatever they actions they need
        to do.

        Alternatively, a ButtonListener can be added to the button, and these listeners
        will be called when the click occurs.

        @see triggerClick
    */
    virtual void clicked();

    /** This method is called when the button has been clicked.

        By default it just calls clicked(), but you might want to override it to handle
        things like clicking when a modifier key is pressed, etc.

        @see ModifierKeys
    */
    virtual void clicked (const ModifierKeys& modifiers);

    /** Subclasses should override this to actually paint the button's contents.

        It's better to use this than the paint method, because it gives you information
        about the over/down state of the button.

        @param g                    the graphics context to use
        @param isMouseOverButton    true if the button is either in the 'over' or
                                    'down' state
        @param isButtonDown         true if the button should be drawn in the 'down' position
    */
    virtual void paintButton (Graphics& g,
                              bool isMouseOverButton,
                              bool isButtonDown) = 0;

    /** Called when the button's up/down/over state changes.

        Subclasses can override this if they need to do something special when the button
        goes up or down.

        @see isDown, isOver
    */
    virtual void buttonStateChanged();


    //==============================================================================
    /** @internal */
    virtual void internalClickCallback (const ModifierKeys& modifiers);
    /** @internal */
    void handleCommandMessage (int commandId);
    /** @internal */
    void mouseEnter (const MouseEvent& e);
    /** @internal */
    void mouseExit (const MouseEvent& e);
    /** @internal */
    void mouseDown (const MouseEvent& e);
    /** @internal */
    void mouseDrag (const MouseEvent& e);
    /** @internal */
    void mouseUp (const MouseEvent& e);
    /** @internal */
    bool keyPressed (const KeyPress& key);
    /** @internal */
    bool keyPressed (const KeyPress& key, Component* originatingComponent);
    /** @internal */
    bool keyStateChanged (Component* originatingComponent);
    /** @internal */
    void paint (Graphics& g);
    /** @internal */
    void parentHierarchyChanged();
    /** @internal */
    void focusGained (FocusChangeType cause);
    /** @internal */
    void focusLost (FocusChangeType cause);
    /** @internal */
    void enablementChanged();
    /** @internal */
    void applicationCommandInvoked (const ApplicationCommandTarget::InvocationInfo&);
    /** @internal */
    void applicationCommandListChanged();


private:
    //==============================================================================
    Array <KeyPress> shortcuts;
    Component* keySource;
    String text;
    SortedSet <void*> buttonListeners;

    friend class InternalButtonRepeatTimer;
    Timer* repeatTimer;
    uint32 buttonPressTime, lastTimeCallbackTime;
    ApplicationCommandManager* commandManagerToUse;
    int autoRepeatDelay, autoRepeatSpeed, autoRepeatMinimumDelay;
    int radioGroupId, commandID, connectedEdgeFlags;
    ButtonState buttonState;

    bool isOn : 1;
    bool clickTogglesState : 1;
    bool needsToRelease : 1;
    bool needsRepainting : 1;
    bool isKeyDown : 1;
    bool triggerOnMouseDown : 1;
    bool generateTooltip : 1;

    void repeatTimerCallback() throw();
    Timer& getRepeatTimer() throw();

    ButtonState updateState (const MouseEvent* const e) throw();
    bool isShortcutPressed() const throw();
    void turnOffOtherButtonsInGroup (const bool sendChangeNotification);

    void flashButtonState() throw();
    void sendClickMessage (const ModifierKeys& modifiers);
    void sendStateMessage();

    Button (const Button&);
    const Button& operator= (const Button&);
};


#endif   // __JUCE_BUTTON_JUCEHEADER__
