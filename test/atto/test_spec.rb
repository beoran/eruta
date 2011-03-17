require 'atto'

include Atto::Spec

Feature "a specification should support steps" do
  Scenario "inside a feature dedinition" do
    Given "the framework is ready"
    When "a specification step is defined"
    Then "it should be added to the framework" do
      true
    end
  end
end  

Feature "another feature" do
  Scenario "inside a feature dedinition" do
    Given "the framework is ready" do
      @foo = 20 
    end
    When "a specification step is defined" do 
      @foo = 10
    end
    Then "it should be added to the framework" do
      @foo == 11
    end
  end
end  

puts Atto::Spec.describe_all
    



