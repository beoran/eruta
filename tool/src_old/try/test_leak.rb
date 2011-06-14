module Play
  class Toy
    def peek_at_proc
      puts `ps u -p #{Process.pid}`
    end

    def base_name
      # x = 1 # <<<<<< REMOVE COMMENT TO FIX MEM LEAK
      "a,b".split(',')
    end

    def test
      peek_at_proc

      10000000.times do
        base_name
      end

      peek_at_proc
    end
  end
end

Play::Toy.new.test 