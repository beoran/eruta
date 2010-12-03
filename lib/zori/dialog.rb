# encoding: UTF-8
#
# Dialog, a class for standard dialogs
#

module Zori  
  class Dialog < Frame
    def initialize(*args, &block)
      super(*args, &block)
      # @z            = 10000
      @style        = self.style.deep_copy
      # @style.colors.background  = Sisa::Color::Cyan
    end
    
    # Move the dialog to the top of the screen
    def to_top
      self.x = 0
    end
    
    # Centers the dialog horizontally on the screen.
    def to_horizontal_center
      self.x        = self.hanao.w / 2 - self.out_w / 2
    end
    
    # Centers the dialog vertically on the screen.
    def to_vertical_center
      self.y        = self.hanao.h / 2 - self.out_h / 2
    end
    
     # Centers the dialog vertically and horizontally on the screen.
    def to_center 
      to_horizontal_center
      to_vertical_center
    end

    
    # Waits for ourself to be to be finished
    def wait_on(&block)
      self.hanao.queue.poll { |e| } 
      # Empty the event queue to prevent any 
      # lingering events influencing this new dialog.            
      until self.hanao.done? or self.close? do
        self.hanao.update
        self.hanao.draw(self.hanao.screen)
        self.hanao.screen.flip
        block.call(self) if block
      end 
      return self.value if self.respond_to? :value
      return nil
    end
    
    # Adds child with the right priority
    def add_child(widget)
      super(widget)
      # widget.z = @z + 10
    end
    
    alias :<< :add_child
    
    # A choice dialog
    class Choice < Dialog
      attr_accessor :value
      
      # Adds a button to this dialog
      def make_button_action(val)
        dialog = self
        result = lambda do     
            dialog.value = val            
            close
        end
        return result
      end

      # Initializes the dialog with a text and the buttons that it should display, and values it should return 
      def initialize(text, *options, &block)
        super(&block)
        @buttons      = []
        @actions      = []
        # make label
        @label        = Zori::Label.new(text)
        self          << @label
        @value        = nil
        self.layout_all
        # Add buttons
        for pair  in options do
          val, text = *pair
          action    = make_button_action(val)
          @actions << action
          button    = Zori::Button.new(text, &action)
          @buttons << button
          self     << button 
        end
        self.layout_all
        self.fit_child_high_simple
        self.fit_child_wide_simple
        to_center
      end
      
    end
    
    # A dialog that asks a simple text question 
    class Question < Dialog
      attr_accessor :value
      
      def make_ok_action()
        dialog = self
        result = lambda do     
            dialog.value = @input.value
            close
        end
      end
      
      def make_cancel_action()
        dialog = self
        result = lambda do     
            dialog.value = nil
            close
        end
      end
      
      def initialize(question, text = '')
        super() {}        
        @actions      = []
        @label        = Zori::Label.new(question)
        @cancel_button= Zori::Button.new('Cancel', &make_cancel_action) 
        @ok_button    = Zori::Button.new('OK'    , &make_ok_action) 
        @input        = Zori::Input.new('', &make_ok_action)
                
        self        << @label
        @value        = nil
        self        << @input
        self        << @cancel_button
        self        << @ok_button
        self.layout_all
        self.fit_child_high_simple
        self.fit_child_wide_simple
        @input.focus!
        # Focus on the input text
        to_center
      end
      
    end

    # A Textsplash is a dialog that displays a message at the top 
    # or bottom of the screen and then closes itself after a given time interval
    class Textsplash < Dialog
      def initialize(message, delay = 3.0)
        super() {}
        @delay        = delay
        @actions      = []
        @label        = Zori::Label.new(message)        
        self         << @label
        self.layout_all
        self.fit_child_high_simple
        self.fit_child_wide_simple 
        @start        = Time.now()
        # to_top
        to_center
      end
      
      # Draws the widget, but closes it it's time is up.
      def draw(display)
        super(display)        
        if (Time.now()-@start) > @delay
          close
        end
      end
      
    end   
    
    # A Status is a dialog that displays a message at the top 
    # or bottom of the screen
    class Status < Dialog
      include Flowlayout
      
      def initialize(text_in, *args, &block)
        @text         = text_in                
        @actions      = []
        @label        = Zori::Label.new(@text)                        
        super(*args, &block)
        self         << @label        
        self.update()
        to_top
        to_horizontal_center
      end
           
      def update
        super
        self.w        = self.hanao.w
        @label.w      = self.in_w
        @label.heading = @text
        @label.update
        self.layout_all
        self.fit_child_wide_simple
        self.fit_child_high_simple
      end
      
      def text 
        return @text
      end
      
      def text=(text_in)
        @text = text_in
        update()
      end
            
    end  
    
    # Creates a choice dialog
    def self.new_choose(message, *options)
      dialog = Dialog::Choice.new(message, *options)
      return dialog     
    end
    
    # Creates a yes/no choice dialog
    def self.new_yesno(message)
      dialog = Dialog::Choice.new(message, [true, 'Yes'], [false, 'No'])
      return dialog     
    end
    
    # Creates a cancel/ok choice dialog
    def self.new_cancelok(message)
      dialog = Dialog::Choice.new(message, [false, 'Cancel'], [true, 'Ok'])
      return dialog     
    end
    
    # Creates an alert dialog
    def self.new_alert(message, text='')
      dialog = Dialog::Choice.new(message, text)
      return dialog     
    end    
    
    # Creates an ask dialog (for simple text input)
    def self.new_ask(question, text="")
      dialog = Dialog::Question.new(question, text)
      return dialog     
    end    
    
    # Creates an textsplash dialog (for timed text output)
    def self.new_textsplash(message="", delay=3.0)
      dialog = Dialog::Textsplash.new(message, delay)
      return dialog     
    end    
    
    # Creates a new status dialog 
    def self.new_status(message="", &block)
      dialog = Dialog::Status.new(message, &block)
      return dialog     
    end    
    
    
    # Inserts dialog into the system
    def self.into_system(dialog)
      system       = Zori::Hanao.current
      system.main.add_dialog(dialog)
      # Add to dialog pane
      dialog.to_front
      # And ensure it is set to the front. 
    end    
        
    # Creates a chooice dialog and waits until the user presses one of 
    # it's buttons.
    def self.choose(message, *options, &block)
      dialog = Dialog.new_choose(message, *options)
      into_system(dialog)
      dialog.wait_on(&block)      
    end
    
    # Creates a tyes/no dialog and waits until the user presses one of 
    # it's buttons.
    def self.yesno(message, &block)
      dialog = Dialog.new_yesno(message)
      into_system(dialog)
      dialog.wait_on(&block)      
    end
    
    # Creates a cancel/ok dialog and waits until the user presses one of 
    # it's buttons.
    def self.cancelok(message, &block)
      dialog = Dialog.new_cancelok(message)
      into_system(dialog)
      dialog.wait_on(&block)      
    end
    
    # Creates an alert dialog and waits until the user presses one of 
    # it's buttons.
    def self.alert(message, &block)
      dialog = Dialog.new_alert(message)
      into_system(dialog)
      dialog.wait_on(&block)      
    end
    
    # Creates an ask dialog and waits until the user presses one of 
    # it's buttons.
    def self.ask(message='', text='', &block)
      dialog = Dialog.new_ask(message, text)
      into_system(dialog)
      dialog.wait_on(&block)      
    end
    
    # Creates a status dialog and inserts it into the GUI automatically
    # Returns the status dialog created. 
    def self.status(message='',&block)
      dialog = Dialog.new_status(message, &block)
      into_system(dialog)
      return dialog
    end

    
    # Creates an ask dialog and waits until the delay has passed.
    # The block will be continuously called, with the dialog as it's parameter
    # while the textsplash is showing
    def self.textsplash(message, delay=3.0, &block)
      dialog = Dialog.new_textsplash(message, delay)
      into_system(dialog)
      dialog.wait_on(&block)
    end
    
  end
end


