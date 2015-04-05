module Zori
  # Module for classes that do not have or ignore their children
  module Fixedlayout 
    # Fixedlayout doesn't shrink even if requested to.
    def fit_child_high()
      return self
    end
    
    # Fixedlayout doesn't shrink even if requested to.
    def fit_child_high_simple()
      return self
    end
    
    # Fixedlayout doesn't shrink even if requested to.
    def fit_child_wide_simple()
      return self
    end
    
    # Fixedlayout doesn't shrink even if requested to.
    def fit_wide(widget=nil) 
    end
    
    # Fixedlayout doesn't shrink even if requested to.
    def fit_high(widget=nil) 
    end
  end
end