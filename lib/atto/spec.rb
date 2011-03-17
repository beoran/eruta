require 'timeout'

module Atto
  module Spec
  
    def Feature(text, &block)
      @@features ||= []
      @@feature = Struct.new(:text, :block, :scenarios).new(text, block, [])
      @@features << @@feature
      block.call
      return @@feature
    end
    
    def Scenario(text, &block)
      @@scenario = Struct.new(:text, :block, :steps).new(text, block, [])
      @@feature.scenarios << @@scenario
      block.call      
      return @@scenario      
    end
    
    def Step(text, kind, block, stack = caller)
      step = Struct.new(:text, :kind, :block, :stack).new(text, kind, block, stack)
      @@scenario.steps << step
      return step
    end
    
    def Given(text, &block)
      return Step(text, :given, block) 
    end
    
    def When(text, &block)
      return Step(text, :when, block)
    end
    
    def Then(text, &block)
      return Step(text, :then, block)
    end
    
    def describe_step(feature, scenario, step)
      "\nFeature " + feature.text + ".\n  Scenario " + scenario.text +
      ".\n    #{step.kind.to_s.capitalize} " + step.text + "."
    end

    def run_step(feature, scenario, step, wait = 10, context = caller)
      return unless step.block
      res, raised = Test.run_block(wait, &step.block) 
      msg         = describe_step(feature, scenario, step)
      failure     = Test.make_failure(res, raised, msg, step.stack)
      @@failures << failure if failure
    end
    
    def run(wait=10, context = caller)
      @@failures = []      
      for feature in @@features
        for scenario in feature.scenarios
          for step in scenario.steps
            run_step(feature, scenario, step, wait)
          end
        end
      end
    end
    
    def describe_all
      res = ''
      for feature in @@features
        res << "Feature " + feature.text + "."
        for scenario in feature.scenarios
        res << "\n  Scenario " + scenario.text + "."
          for step in scenario.steps
            res << "\n    #{step.kind.to_s.capitalize} " + step.text + "."
          end
        end
      end
      return res
    end

    def results 
      aid = @@failures.join("\n") + "\n"
      col = @@failures.empty? ? :green : :red
      aid << Atto::Ansi.color_string(col, "=" * 78)
    end
    
    def on_exit
      self.run
      puts results unless results.strip.empty?; exit @@failures.empty? 
    end
  
    at_exit { on_exit }

  end
end  