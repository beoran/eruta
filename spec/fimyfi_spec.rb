
require 'helper'
require 'fimyfi'

describe Fimyfi, "resource files finder" do
  it "Should find the resource directory" do
    d = Fimyfi.dir('eruta')
    d.should_not be_nil
    Fimyfi.font_dir.should_not be_nil
  end

end
