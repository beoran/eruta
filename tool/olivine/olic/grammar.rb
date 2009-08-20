module Olivine

  module Grammar
    include Treetop::Runtime

    def root
      @root || :unit
    end

    def _nt_unit
      start_index = index
      if node_cache[:unit].has_key?(index)
        cached = node_cache[:unit][index]
        @index = cached.interval.end if cached
        return cached
      end

      s0, i0 = [], index
      loop do
        i1 = index
        r2 = _nt_various_list
        if r2
          r1 = r2
        else
          r3 = _nt_blank_comment
          if r3
            r1 = r3
          else
            self.index = i1
            r1 = nil
          end
        end
        if r1
          s0 << r1
        else
          break
        end
      end
      r0 = instantiate_node(SyntaxNode,input, i0...index, s0)

      node_cache[:unit][start_index] = r0

      return r0
    end

    def _nt_peol
      start_index = index
      if node_cache[:peol].has_key?(index)
        cached = node_cache[:peol][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      if input.index("\r\n", index) == index
        r1 = instantiate_node(SyntaxNode,input, index...(index + 2))
        @index += 2
      else
        terminal_parse_failure("\r\n")
        r1 = nil
      end
      if r1
        r0 = r1
      else
        if input.index("\n", index) == index
          r2 = instantiate_node(SyntaxNode,input, index...(index + 1))
          @index += 1
        else
          terminal_parse_failure("\n")
          r2 = nil
        end
        if r2
          r0 = r2
        else
          if input.index("\r", index) == index
            r3 = instantiate_node(Node::Boring,input, index...(index + 1))
            @index += 1
          else
            terminal_parse_failure("\r")
            r3 = nil
          end
          if r3
            r0 = r3
          else
            self.index = i0
            r0 = nil
          end
        end
      end

      node_cache[:peol][start_index] = r0

      return r0
    end

    module EscEol0
      def peol
        elements[1]
      end
    end

    def _nt_esc_eol
      start_index = index
      if node_cache[:esc_eol].has_key?(index)
        cached = node_cache[:esc_eol][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      if input.index('\\', index) == index
        r1 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure('\\')
        r1 = nil
      end
      s0 << r1
      if r1
        r2 = _nt_peol
        s0 << r2
      end
      if s0.last
        r0 = instantiate_node(Node::Boring,input, i0...index, s0)
        r0.extend(EscEol0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:esc_eol][start_index] = r0

      return r0
    end

    module Eol0
      def peol
        elements[1]
      end
    end

    def _nt_eol
      start_index = index
      if node_cache[:eol].has_key?(index)
        cached = node_cache[:eol][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      i1 = index
      if input.index('\\', index) == index
        r2 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure('\\')
        r2 = nil
      end
      if r2
        r1 = nil
      else
        self.index = i1
        r1 = instantiate_node(SyntaxNode,input, index...index)
      end
      s0 << r1
      if r1
        r3 = _nt_peol
        s0 << r3
      end
      if s0.last
        r0 = instantiate_node(Node::Boring,input, i0...index, s0)
        r0.extend(Eol0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:eol][start_index] = r0

      return r0
    end

    module Neol0
    end

    def _nt_neol
      start_index = index
      if node_cache[:neol].has_key?(index)
        cached = node_cache[:neol][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      i1 = index
      r2 = _nt_eol
      if r2
        r1 = nil
      else
        self.index = i1
        r1 = instantiate_node(SyntaxNode,input, index...index)
      end
      s0 << r1
      if r1
        i3 = index
        r4 = _nt_esc_eol
        if r4
          r3 = r4
        else
          if index < input_length
            r5 = instantiate_node(SyntaxNode,input, index...(index + 1))
            @index += 1
          else
            terminal_parse_failure("any character")
            r5 = nil
          end
          if r5
            r3 = r5
          else
            self.index = i3
            r3 = nil
          end
        end
        s0 << r3
      end
      if s0.last
        r0 = instantiate_node(Node::Boring,input, i0...index, s0)
        r0.extend(Neol0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:neol][start_index] = r0

      return r0
    end

    module LogicalLine0
      def eol
        elements[1]
      end
    end

    def _nt_logical_line
      start_index = index
      if node_cache[:logical_line].has_key?(index)
        cached = node_cache[:logical_line][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      s1, i1 = [], index
      loop do
        r2 = _nt_neol
        if r2
          s1 << r2
        else
          break
        end
      end
      r1 = instantiate_node(SyntaxNode,input, i1...index, s1)
      s0 << r1
      if r1
        r3 = _nt_eol
        s0 << r3
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(LogicalLine0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:logical_line][start_index] = r0

      return r0
    end

    def _nt_ws
      start_index = index
      if node_cache[:ws].has_key?(index)
        cached = node_cache[:ws][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_esc_eol
      if r1
        r0 = r1
      else
        if input.index(Regexp.new('[ \\t]'), index) == index
          r2 = instantiate_node(SyntaxNode,input, index...(index + 1))
          @index += 1
        else
          r2 = nil
        end
        if r2
          r0 = r2
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:ws][start_index] = r0

      return r0
    end

    def _nt_sp
      start_index = index
      if node_cache[:sp].has_key?(index)
        cached = node_cache[:sp][index]
        @index = cached.interval.end if cached
        return cached
      end

      s0, i0 = [], index
      loop do
        r1 = _nt_ws
        if r1
          s0 << r1
        else
          break
        end
      end
      if s0.empty?
        self.index = i0
        r0 = nil
      else
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
      end

      node_cache[:sp][start_index] = r0

      return r0
    end

    def _nt_block_comment_open
      start_index = index
      if node_cache[:block_comment_open].has_key?(index)
        cached = node_cache[:block_comment_open][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index('/*', index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 2))
        @index += 2
      else
        terminal_parse_failure('/*')
        r0 = nil
      end

      node_cache[:block_comment_open][start_index] = r0

      return r0
    end

    def _nt_block_comment_close
      start_index = index
      if node_cache[:block_comment_close].has_key?(index)
        cached = node_cache[:block_comment_close][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index('*/', index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 2))
        @index += 2
      else
        terminal_parse_failure('*/')
        r0 = nil
      end

      node_cache[:block_comment_close][start_index] = r0

      return r0
    end

    module BlockCommentContent0
    end

    def _nt_block_comment_content
      start_index = index
      if node_cache[:block_comment_content].has_key?(index)
        cached = node_cache[:block_comment_content][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      i1 = index
      r2 = _nt_block_comment_close
      if r2
        r1 = nil
      else
        self.index = i1
        r1 = instantiate_node(SyntaxNode,input, index...index)
      end
      s0 << r1
      if r1
        i3 = index
        r4 = _nt_block_comment
        if r4
          r3 = r4
        else
          if index < input_length
            r5 = instantiate_node(SyntaxNode,input, index...(index + 1))
            @index += 1
          else
            terminal_parse_failure("any character")
            r5 = nil
          end
          if r5
            r3 = r5
          else
            self.index = i3
            r3 = nil
          end
        end
        s0 << r3
      end
      if s0.last
        r0 = instantiate_node(Node::Boring,input, i0...index, s0)
        r0.extend(BlockCommentContent0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:block_comment_content][start_index] = r0

      return r0
    end

    module BlockComment0
      def block_comment_open
        elements[0]
      end

      def block_comment_close
        elements[2]
      end
    end

    def _nt_block_comment
      start_index = index
      if node_cache[:block_comment].has_key?(index)
        cached = node_cache[:block_comment][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_block_comment_open
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          r3 = _nt_block_comment_content
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
        if r2
          r4 = _nt_block_comment_close
          s0 << r4
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(BlockComment0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:block_comment][start_index] = r0

      return r0
    end

    module LineComment0
    end

    def _nt_line_comment
      start_index = index
      if node_cache[:line_comment].has_key?(index)
        cached = node_cache[:line_comment][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      if input.index('#', index) == index
        r1 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure('#')
        r1 = nil
      end
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          r3 = _nt_neol
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
        if r2
          i4 = index
          r5 = _nt_eol
          if r5
            self.index = i4
            r4 = instantiate_node(SyntaxNode,input, index...index)
          else
            r4 = nil
          end
          s0 << r4
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(LineComment0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:line_comment][start_index] = r0

      return r0
    end

    def _nt_comment
      start_index = index
      if node_cache[:comment].has_key?(index)
        cached = node_cache[:comment][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_block_comment
      if r1
        r0 = r1
      else
        r2 = _nt_line_comment
        if r2
          r0 = r2
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:comment][start_index] = r0

      return r0
    end

    def _nt_blank
      start_index = index
      if node_cache[:blank].has_key?(index)
        cached = node_cache[:blank][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_ws
      if r1
        r0 = r1
      else
        r2 = _nt_eol
        if r2
          r0 = r2
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:blank][start_index] = r0

      return r0
    end

    def _nt_blank_comment
      start_index = index
      if node_cache[:blank_comment].has_key?(index)
        cached = node_cache[:blank_comment][index]
        @index = cached.interval.end if cached
        return cached
      end

      s0, i0 = [], index
      loop do
        i1 = index
        r2 = _nt_comment
        if r2
          r1 = r2
        else
          r3 = _nt_blank
          if r3
            r1 = r3
          else
            self.index = i1
            r1 = nil
          end
        end
        if r1
          s0 << r1
        else
          break
        end
      end
      if s0.empty?
        self.index = i0
        r0 = nil
      else
        r0 = instantiate_node(Node::Boring,input, i0...index, s0)
      end

      node_cache[:blank_comment][start_index] = r0

      return r0
    end

    def _nt_eos
      start_index = index
      if node_cache[:eos].has_key?(index)
        cached = node_cache[:eos][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_eol
      if r1
        r0 = r1
      else
        if input.index(';', index) == index
          r2 = instantiate_node(SyntaxNode,input, index...(index + 1))
          @index += 1
        else
          terminal_parse_failure(';')
          r2 = nil
        end
        if r2
          r0 = r2
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:eos][start_index] = r0

      return r0
    end

    def _nt_eof
      start_index = index
      if node_cache[:eof].has_key?(index)
        cached = node_cache[:eof][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      if index < input_length
        r1 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure("any character")
        r1 = nil
      end
      if r1
        r0 = nil
      else
        self.index = i0
        r0 = instantiate_node(SyntaxNode,input, index...index)
      end

      node_cache[:eof][start_index] = r0

      return r0
    end

    def _nt_empty
      start_index = index
      if node_cache[:empty].has_key?(index)
        cached = node_cache[:empty][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index('', index) == index
        r0 = instantiate_node(SyntaxNode,input, index...(index + 0))
        @index += 0
      else
        terminal_parse_failure('')
        r0 = nil
      end

      node_cache[:empty][start_index] = r0

      return r0
    end

    def _nt_message
      start_index = index
      if node_cache[:message].has_key?(index)
        cached = node_cache[:message][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_string_message
      if r1
        r0 = r1
      else
        r2 = _nt_symbol_message
        if r2
          r0 = r2
        else
          r3 = _nt_number_message
          if r3
            r0 = r3
          else
            r4 = _nt_operator_message
            if r4
              r0 = r4
            else
              r5 = _nt_plain_message
              if r5
                r0 = r5
              else
                self.index = i0
                r0 = nil
              end
            end
          end
        end
      end

      node_cache[:message][start_index] = r0

      return r0
    end

    def _nt_various_list
      start_index = index
      if node_cache[:various_list].has_key?(index)
        cached = node_cache[:various_list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_closed_space_list
      if r1
        r0 = r1
      else
        r2 = _nt_open_space_list
        if r2
          r0 = r2
        else
          r3 = _nt_closed_comma_list
          if r3
            r0 = r3
          else
            r4 = _nt_open_comma_list
            if r4
              r0 = r4
            else
              self.index = i0
              r0 = nil
            end
          end
        end
      end

      node_cache[:various_list][start_index] = r0

      return r0
    end

    def _nt_open_brackets
      start_index = index
      if node_cache[:open_brackets].has_key?(index)
        cached = node_cache[:open_brackets][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index('(', index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure('(')
        r0 = nil
      end

      node_cache[:open_brackets][start_index] = r0

      return r0
    end

    def _nt_close_brackets
      start_index = index
      if node_cache[:close_brackets].has_key?(index)
        cached = node_cache[:close_brackets][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index(')', index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure(')')
        r0 = nil
      end

      node_cache[:close_brackets][start_index] = r0

      return r0
    end

    module Owsd0
      def empty
        elements[0]
      end

    end

    module Owsd1
      def empty
        elements[0]
      end

    end

    def _nt_owsd
      start_index = index
      if node_cache[:owsd].has_key?(index)
        cached = node_cache[:owsd][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_ws
      r1.extend(Modules::Boring)
      if r1
        r0 = r1
      else
        i2, s2 = index, []
        r3 = _nt_empty
        s2 << r3
        if r3
          i4 = index
          r5 = _nt_open_brackets
          if r5
            self.index = i4
            r4 = instantiate_node(SyntaxNode,input, index...index)
          else
            r4 = nil
          end
          s2 << r4
        end
        if s2.last
          r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
          r2.extend(Owsd0)
        else
          self.index = i2
          r2 = nil
        end
        if r2
          r0 = r2
        else
          i6, s6 = index, []
          r7 = _nt_empty
          s6 << r7
          if r7
            i8 = index
            r9 = _nt_close_brackets
            if r9
              self.index = i8
              r8 = instantiate_node(SyntaxNode,input, index...index)
            else
              r8 = nil
            end
            s6 << r8
          end
          if s6.last
            r6 = instantiate_node(SyntaxNode,input, i6...index, s6)
            r6.extend(Owsd1)
          else
            self.index = i6
            r6 = nil
          end
          if r6
            r0 = r6
          else
            self.index = i0
            r0 = nil
          end
        end
      end

      node_cache[:owsd][start_index] = r0

      return r0
    end

    def _nt_cwsd
      start_index = index
      if node_cache[:cwsd].has_key?(index)
        cached = node_cache[:cwsd][index]
        @index = cached.interval.end if cached
        return cached
      end

      r0 = _nt_blank
      r0.extend(Modules::Boring)

      node_cache[:cwsd][start_index] = r0

      return r0
    end

    module OpenSpaceList0
      def listpart
        elements[1]
      end
    end

    module OpenSpaceList1
      def listpart
        elements[0]
      end

      def eos
        elements[3]
      end
    end

    def _nt_open_space_list
      start_index = index
      if node_cache[:open_space_list].has_key?(index)
        cached = node_cache[:open_space_list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_listpart
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          i3, s3 = index, []
          s4, i4 = [], index
          loop do
            r5 = _nt_owsd
            if r5
              s4 << r5
            else
              break
            end
          end
          if s4.empty?
            self.index = i4
            r4 = nil
          else
            r4 = instantiate_node(SyntaxNode,input, i4...index, s4)
          end
          s3 << r4
          if r4
            r6 = _nt_listpart
            s3 << r6
          end
          if s3.last
            r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
            r3.extend(OpenSpaceList0)
          else
            self.index = i3
            r3 = nil
          end
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
        if r2
          s7, i7 = [], index
          loop do
            r8 = _nt_ws
            if r8
              s7 << r8
            else
              break
            end
          end
          r7 = instantiate_node(SyntaxNode,input, i7...index, s7)
          s0 << r7
          if r7
            r9 = _nt_eos
            s0 << r9
          end
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(OpenSpaceList1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:open_space_list][start_index] = r0

      return r0
    end

    module ClosedSpaceList0
      def listpart
        elements[1]
      end
    end

    module ClosedSpaceList1
      def open_brackets
        elements[0]
      end

      def listpart
        elements[2]
      end

      def close_brackets
        elements[5]
      end
    end

    def _nt_closed_space_list
      start_index = index
      if node_cache[:closed_space_list].has_key?(index)
        cached = node_cache[:closed_space_list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_open_brackets
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          r3 = _nt_ws
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
        if r2
          r4 = _nt_listpart
          s0 << r4
          if r4
            s5, i5 = [], index
            loop do
              i6, s6 = index, []
              s7, i7 = [], index
              loop do
                r8 = _nt_cwsd
                if r8
                  s7 << r8
                else
                  break
                end
              end
              if s7.empty?
                self.index = i7
                r7 = nil
              else
                r7 = instantiate_node(SyntaxNode,input, i7...index, s7)
              end
              s6 << r7
              if r7
                r9 = _nt_listpart
                s6 << r9
              end
              if s6.last
                r6 = instantiate_node(SyntaxNode,input, i6...index, s6)
                r6.extend(ClosedSpaceList0)
              else
                self.index = i6
                r6 = nil
              end
              if r6
                s5 << r6
              else
                break
              end
            end
            r5 = instantiate_node(SyntaxNode,input, i5...index, s5)
            s0 << r5
            if r5
              s10, i10 = [], index
              loop do
                r11 = _nt_ws
                if r11
                  s10 << r11
                else
                  break
                end
              end
              r10 = instantiate_node(SyntaxNode,input, i10...index, s10)
              s0 << r10
              if r10
                r12 = _nt_close_brackets
                s0 << r12
              end
            end
          end
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(ClosedSpaceList1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:closed_space_list][start_index] = r0

      return r0
    end

    def _nt_comma
      start_index = index
      if node_cache[:comma].has_key?(index)
        cached = node_cache[:comma][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index(',', index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure(',')
        r0 = nil
      end

      node_cache[:comma][start_index] = r0

      return r0
    end

    module OpenCommaList0
      def comma
        elements[1]
      end

      def listpart
        elements[3]
      end
    end

    module OpenCommaList1
      def listpart
        elements[0]
      end

      def eos
        elements[3]
      end
    end

    def _nt_open_comma_list
      start_index = index
      if node_cache[:open_comma_list].has_key?(index)
        cached = node_cache[:open_comma_list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_listpart
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          i3, s3 = index, []
          s4, i4 = [], index
          loop do
            r5 = _nt_ws
            if r5
              s4 << r5
            else
              break
            end
          end
          r4 = instantiate_node(SyntaxNode,input, i4...index, s4)
          s3 << r4
          if r4
            r6 = _nt_comma
            s3 << r6
            if r6
              s7, i7 = [], index
              loop do
                r8 = _nt_blank
                if r8
                  s7 << r8
                else
                  break
                end
              end
              r7 = instantiate_node(SyntaxNode,input, i7...index, s7)
              s3 << r7
              if r7
                r9 = _nt_listpart
                s3 << r9
              end
            end
          end
          if s3.last
            r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
            r3.extend(OpenCommaList0)
          else
            self.index = i3
            r3 = nil
          end
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
        if r2
          s10, i10 = [], index
          loop do
            r11 = _nt_ws
            if r11
              s10 << r11
            else
              break
            end
          end
          r10 = instantiate_node(SyntaxNode,input, i10...index, s10)
          s0 << r10
          if r10
            r12 = _nt_eos
            s0 << r12
          end
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(OpenCommaList1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:open_comma_list][start_index] = r0

      return r0
    end

    module ClosedCommaList0
      def comma
        elements[1]
      end

      def listpart
        elements[3]
      end
    end

    module ClosedCommaList1
      def open_brackets
        elements[0]
      end

      def listpart
        elements[1]
      end

      def close_brackets
        elements[4]
      end
    end

    def _nt_closed_comma_list
      start_index = index
      if node_cache[:closed_comma_list].has_key?(index)
        cached = node_cache[:closed_comma_list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_open_brackets
      s0 << r1
      if r1
        r2 = _nt_listpart
        s0 << r2
        if r2
          s3, i3 = [], index
          loop do
            i4, s4 = index, []
            s5, i5 = [], index
            loop do
              r6 = _nt_ws
              if r6
                s5 << r6
              else
                break
              end
            end
            r5 = instantiate_node(SyntaxNode,input, i5...index, s5)
            s4 << r5
            if r5
              r7 = _nt_comma
              s4 << r7
              if r7
                s8, i8 = [], index
                loop do
                  r9 = _nt_blank
                  if r9
                    s8 << r9
                  else
                    break
                  end
                end
                r8 = instantiate_node(SyntaxNode,input, i8...index, s8)
                s4 << r8
                if r8
                  r10 = _nt_listpart
                  s4 << r10
                end
              end
            end
            if s4.last
              r4 = instantiate_node(SyntaxNode,input, i4...index, s4)
              r4.extend(ClosedCommaList0)
            else
              self.index = i4
              r4 = nil
            end
            if r4
              s3 << r4
            else
              break
            end
          end
          r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
          s0 << r3
          if r3
            s11, i11 = [], index
            loop do
              r12 = _nt_ws
              if r12
                s11 << r12
              else
                break
              end
            end
            r11 = instantiate_node(SyntaxNode,input, i11...index, s11)
            s0 << r11
            if r11
              r13 = _nt_close_brackets
              s0 << r13
            end
          end
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(ClosedCommaList1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:closed_comma_list][start_index] = r0

      return r0
    end

    def _nt_listpart
      start_index = index
      if node_cache[:listpart].has_key?(index)
        cached = node_cache[:listpart][index]
        @index = cached.interval.end if cached
        return cached
      end

      r0 = _nt_message

      node_cache[:listpart][start_index] = r0

      return r0
    end

    module ListSeparator0
    end

    def _nt_list_separator
      start_index = index
      if node_cache[:list_separator].has_key?(index)
        cached = node_cache[:list_separator][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      s1, i1 = [], index
      loop do
        r2 = _nt_whitespace
        if r2
          s1 << r2
        else
          break
        end
      end
      r1 = instantiate_node(SyntaxNode,input, i1...index, s1)
      s0 << r1
      if r1
        if input.index(',', index) == index
          r3 = instantiate_node(SyntaxNode,input, index...(index + 1))
          @index += 1
        else
          terminal_parse_failure(',')
          r3 = nil
        end
        s0 << r3
        if r3
          s4, i4 = [], index
          loop do
            r5 = _nt_whitespace
            if r5
              s4 << r5
            else
              break
            end
          end
          r4 = instantiate_node(SyntaxNode,input, i4...index, s4)
          s0 << r4
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(ListSeparator0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:list_separator][start_index] = r0

      return r0
    end

    def _nt_list
      start_index = index
      if node_cache[:list].has_key?(index)
        cached = node_cache[:list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_undelimited_list
      if r1
        r0 = r1
      else
        r2 = _nt_delimited_list
        if r2
          r0 = r2
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:list][start_index] = r0

      return r0
    end

    module UndelimitedList0
      def list_separator
        elements[0]
      end

      def list_element
        elements[1]
      end
    end

    module UndelimitedList1
      def list_element
        elements[0]
      end

    end

    def _nt_undelimited_list
      start_index = index
      if node_cache[:undelimited_list].has_key?(index)
        cached = node_cache[:undelimited_list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_list_element
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          i3, s3 = index, []
          r4 = _nt_list_separator
          s3 << r4
          if r4
            r5 = _nt_list_element
            s3 << r5
          end
          if s3.last
            r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
            r3.extend(UndelimitedList0)
          else
            self.index = i3
            r3 = nil
          end
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(UndelimitedList1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:undelimited_list][start_index] = r0

      return r0
    end

    module DelimitedList0
      def list_separator
        elements[0]
      end

      def list_element
        elements[1]
      end
    end

    module DelimitedList1
      def list_element
        elements[0]
      end

    end

    module DelimitedList2
      def list_start
        elements[0]
      end

      def list_end
        elements[2]
      end
    end

    def _nt_delimited_list
      start_index = index
      if node_cache[:delimited_list].has_key?(index)
        cached = node_cache[:delimited_list][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_list_start
      s0 << r1
      if r1
        i2, s2 = index, []
        r3 = _nt_list_element
        s2 << r3
        if r3
          s4, i4 = [], index
          loop do
            i5, s5 = index, []
            r6 = _nt_list_separator
            s5 << r6
            if r6
              r7 = _nt_list_element
              s5 << r7
            end
            if s5.last
              r5 = instantiate_node(SyntaxNode,input, i5...index, s5)
              r5.extend(DelimitedList0)
            else
              self.index = i5
              r5 = nil
            end
            if r5
              s4 << r5
            else
              break
            end
          end
          r4 = instantiate_node(SyntaxNode,input, i4...index, s4)
          s2 << r4
        end
        if s2.last
          r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
          r2.extend(DelimitedList1)
        else
          self.index = i2
          r2 = nil
        end
        s0 << r2
        if r2
          r8 = _nt_list_end
          s0 << r8
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(DelimitedList2)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:delimited_list][start_index] = r0

      return r0
    end

    def _nt_list_element
      start_index = index
      if node_cache[:list_element].has_key?(index)
        cached = node_cache[:list_element][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_message
      if r1
        r0 = r1
      else
        r2 = _nt_delimited_list
        if r2
          r0 = r2
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:list_element][start_index] = r0

      return r0
    end

    def _nt_chain_separator
      start_index = index
      if node_cache[:chain_separator].has_key?(index)
        cached = node_cache[:chain_separator][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index('.', index) == index
        r0 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure('.')
        r0 = nil
      end

      node_cache[:chain_separator][start_index] = r0

      return r0
    end

    module Chain0
      def chain_separator
        elements[0]
      end

      def chain_element
        elements[1]
      end
    end

    module Chain1
      def chain_element
        elements[0]
      end

    end

    def _nt_chain
      start_index = index
      if node_cache[:chain].has_key?(index)
        cached = node_cache[:chain][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_chain_element
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          i3, s3 = index, []
          r4 = _nt_chain_separator
          s3 << r4
          if r4
            r5 = _nt_chain_element
            s3 << r5
          end
          if s3.last
            r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
            r3.extend(Chain0)
          else
            self.index = i3
            r3 = nil
          end
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(Chain1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:chain][start_index] = r0

      return r0
    end

    def _nt_chain_element
      start_index = index
      if node_cache[:chain_element].has_key?(index)
        cached = node_cache[:chain_element][index]
        @index = cached.interval.end if cached
        return cached
      end

      r0 = _nt_message

      node_cache[:chain_element][start_index] = r0

      return r0
    end

    module EndKeyword0
    end

    module EndKeyword1
    end

    def _nt_end_keyword
      start_index = index
      if node_cache[:end_keyword].has_key?(index)
        cached = node_cache[:end_keyword][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      if input.index("end", index) == index
        r1 = instantiate_node(SyntaxNode,input, index...(index + 3))
        @index += 3
      else
        terminal_parse_failure("end")
        r1 = nil
      end
      s0 << r1
      if r1
        i2 = index
        i3, s3 = index, []
        i4 = index
        if input.index(Regexp.new('[ \\n\\r]'), index) == index
          r5 = instantiate_node(SyntaxNode,input, index...(index + 1))
          @index += 1
        else
          r5 = nil
        end
        if r5
          r4 = nil
        else
          self.index = i4
          r4 = instantiate_node(SyntaxNode,input, index...index)
        end
        s3 << r4
        if r4
          if index < input_length
            r6 = instantiate_node(SyntaxNode,input, index...(index + 1))
            @index += 1
          else
            terminal_parse_failure("any character")
            r6 = nil
          end
          s3 << r6
        end
        if s3.last
          r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
          r3.extend(EndKeyword0)
        else
          self.index = i3
          r3 = nil
        end
        if r3
          r2 = nil
        else
          self.index = i2
          r2 = instantiate_node(SyntaxNode,input, index...index)
        end
        s0 << r2
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(EndKeyword1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:end_keyword][start_index] = r0

      return r0
    end

    module DoKeyword0
    end

    module DoKeyword1
    end

    def _nt_do_keyword
      start_index = index
      if node_cache[:do_keyword].has_key?(index)
        cached = node_cache[:do_keyword][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      if input.index("do", index) == index
        r1 = instantiate_node(SyntaxNode,input, index...(index + 2))
        @index += 2
      else
        terminal_parse_failure("do")
        r1 = nil
      end
      s0 << r1
      if r1
        i2 = index
        i3, s3 = index, []
        i4 = index
        if input.index(Regexp.new('[ \\n\\r]'), index) == index
          r5 = instantiate_node(SyntaxNode,input, index...(index + 1))
          @index += 1
        else
          r5 = nil
        end
        if r5
          r4 = nil
        else
          self.index = i4
          r4 = instantiate_node(SyntaxNode,input, index...index)
        end
        s3 << r4
        if r4
          if index < input_length
            r6 = instantiate_node(SyntaxNode,input, index...(index + 1))
            @index += 1
          else
            terminal_parse_failure("any character")
            r6 = nil
          end
          s3 << r6
        end
        if s3.last
          r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
          r3.extend(DoKeyword0)
        else
          self.index = i3
          r3 = nil
        end
        if r3
          r2 = nil
        else
          self.index = i2
          r2 = instantiate_node(SyntaxNode,input, index...index)
        end
        s0 << r2
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(DoKeyword1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:do_keyword][start_index] = r0

      return r0
    end

    module Block0
      def space_list
        elements[0]
      end

      def spacing
        elements[1]
      end
    end

    module Block1
      def block_body
        elements[1]
      end
    end

    def _nt_block
      start_index = index
      if node_cache[:block].has_key?(index)
        cached = node_cache[:block][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      i2, s2 = index, []
      r3 = _nt_space_list
      s2 << r3
      if r3
        r4 = _nt_spacing
        s2 << r4
      end
      if s2.last
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        r2.extend(Block0)
      else
        self.index = i2
        r2 = nil
      end
      if r2
        r1 = r2
      else
        r1 = instantiate_node(SyntaxNode,input, index...index)
      end
      s0 << r1
      if r1
        r5 = _nt_block_body
        s0 << r5
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(Block1)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:block][start_index] = r0

      return r0
    end

    module BlockBody0
      def do_keyword
        elements[0]
      end

      def block_contents
        elements[1]
      end

      def end_keyword
        elements[2]
      end
    end

    module BlockBody1
      def block_contents
        elements[1]
      end

    end

    def _nt_block_body
      start_index = index
      if node_cache[:block_body].has_key?(index)
        cached = node_cache[:block_body][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      i1, s1 = index, []
      r2 = _nt_do_keyword
      s1 << r2
      if r2
        r3 = _nt_block_contents
        s1 << r3
        if r3
          r4 = _nt_end_keyword
          s1 << r4
        end
      end
      if s1.last
        r1 = instantiate_node(SyntaxNode,input, i1...index, s1)
        r1.extend(BlockBody0)
      else
        self.index = i1
        r1 = nil
      end
      if r1
        r0 = r1
      else
        i5, s5 = index, []
        if input.index("{", index) == index
          r6 = instantiate_node(SyntaxNode,input, index...(index + 1))
          @index += 1
        else
          terminal_parse_failure("{")
          r6 = nil
        end
        s5 << r6
        if r6
          r7 = _nt_block_contents
          s5 << r7
          if r7
            if input.index("}", index) == index
              r8 = instantiate_node(SyntaxNode,input, index...(index + 1))
              @index += 1
            else
              terminal_parse_failure("}")
              r8 = nil
            end
            s5 << r8
          end
        end
        if s5.last
          r5 = instantiate_node(SyntaxNode,input, i5...index, s5)
          r5.extend(BlockBody1)
        else
          self.index = i5
          r5 = nil
        end
        if r5
          r0 = r5
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:block_body][start_index] = r0

      return r0
    end

    def _nt_block_contents
      start_index = index
      if node_cache[:block_contents].has_key?(index)
        cached = node_cache[:block_contents][index]
        @index = cached.interval.end if cached
        return cached
      end

      s0, i0 = [], index
      loop do
        i1 = index
        r2 = _nt_space_list
        if r2
          r1 = r2
        else
          r3 = _nt_spacing
          if r3
            r1 = r3
          else
            r4 = _nt_block
            if r4
              r1 = r4
            else
              self.index = i1
              r1 = nil
            end
          end
        end
        if r1
          s0 << r1
        else
          break
        end
      end
      if s0.empty?
        self.index = i0
        r0 = nil
      else
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
      end

      node_cache[:block_contents][start_index] = r0

      return r0
    end

    def _nt_string_message
      start_index = index
      if node_cache[:string_message].has_key?(index)
        cached = node_cache[:string_message][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_double_quoted_string
      if r1
        r0 = r1
      else
        r2 = _nt_single_quoted_string
        if r2
          r0 = r2
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:string_message][start_index] = r0

      return r0
    end

    module DoubleStringCharacter0
    end

    def _nt_double_string_character
      start_index = index
      if node_cache[:double_string_character].has_key?(index)
        cached = node_cache[:double_string_character][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      i1 = index
      if input.index('"', index) == index
        r2 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure('"')
        r2 = nil
      end
      if r2
        r1 = nil
      else
        self.index = i1
        r1 = instantiate_node(SyntaxNode,input, index...index)
      end
      s0 << r1
      if r1
        i3 = index
        if input.index("\\\\", index) == index
          r4 = instantiate_node(Node::Boring,input, index...(index + 2))
          @index += 2
        else
          terminal_parse_failure("\\\\")
          r4 = nil
        end
        if r4
          r3 = r4
        else
          if input.index('\"', index) == index
            r5 = instantiate_node(Node::Boring,input, index...(index + 2))
            @index += 2
          else
            terminal_parse_failure('\"')
            r5 = nil
          end
          if r5
            r3 = r5
          else
            if index < input_length
              r6 = instantiate_node(Node::Boring,input, index...(index + 1))
              @index += 1
            else
              terminal_parse_failure("any character")
              r6 = nil
            end
            if r6
              r3 = r6
            else
              self.index = i3
              r3 = nil
            end
          end
        end
        s0 << r3
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(DoubleStringCharacter0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:double_string_character][start_index] = r0

      return r0
    end

    def _nt_single_quote
      start_index = index
      if node_cache[:single_quote].has_key?(index)
        cached = node_cache[:single_quote][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index("'", index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure("'")
        r0 = nil
      end

      node_cache[:single_quote][start_index] = r0

      return r0
    end

    def _nt_double_quote
      start_index = index
      if node_cache[:double_quote].has_key?(index)
        cached = node_cache[:double_quote][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index('"', index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure('"')
        r0 = nil
      end

      node_cache[:double_quote][start_index] = r0

      return r0
    end

    module DoubleQuotedString0
      def double_quote
        elements[0]
      end

      def string
        elements[1]
      end

      def double_quote
        elements[2]
      end
    end

    def _nt_double_quoted_string
      start_index = index
      if node_cache[:double_quoted_string].has_key?(index)
        cached = node_cache[:double_quoted_string][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_double_quote
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          r3 = _nt_double_string_character
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
        if r2
          r4 = _nt_double_quote
          s0 << r4
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(DoubleQuotedString0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:double_quoted_string][start_index] = r0

      return r0
    end

    module SingleStringCharacter0
    end

    def _nt_single_string_character
      start_index = index
      if node_cache[:single_string_character].has_key?(index)
        cached = node_cache[:single_string_character][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      i1 = index
      if input.index("'", index) == index
        r2 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure("'")
        r2 = nil
      end
      if r2
        r1 = nil
      else
        self.index = i1
        r1 = instantiate_node(SyntaxNode,input, index...index)
      end
      s0 << r1
      if r1
        i3 = index
        if input.index("\\\\", index) == index
          r4 = instantiate_node(SyntaxNode,input, index...(index + 2))
          @index += 2
        else
          terminal_parse_failure("\\\\")
          r4 = nil
        end
        if r4
          r3 = r4
        else
          if input.index("\\'", index) == index
            r5 = instantiate_node(SyntaxNode,input, index...(index + 2))
            @index += 2
          else
            terminal_parse_failure("\\'")
            r5 = nil
          end
          if r5
            r3 = r5
          else
            if index < input_length
              r6 = instantiate_node(SyntaxNode,input, index...(index + 1))
              @index += 1
            else
              terminal_parse_failure("any character")
              r6 = nil
            end
            if r6
              r3 = r6
            else
              self.index = i3
              r3 = nil
            end
          end
        end
        s0 << r3
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(SingleStringCharacter0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:single_string_character][start_index] = r0

      return r0
    end

    module SingleQuotedString0
      def single_quote
        elements[0]
      end

      def string
        elements[1]
      end

      def single_quote
        elements[2]
      end
    end

    def _nt_single_quoted_string
      start_index = index
      if node_cache[:single_quoted_string].has_key?(index)
        cached = node_cache[:single_quoted_string][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_single_quote
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          r3 = _nt_single_string_character
          if r3
            s2 << r3
          else
            break
          end
        end
        r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        s0 << r2
        if r2
          r4 = _nt_single_quote
          s0 << r4
        end
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(SingleQuotedString0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:single_quoted_string][start_index] = r0

      return r0
    end

    def _nt_alpha_char
      start_index = index
      if node_cache[:alpha_char].has_key?(index)
        cached = node_cache[:alpha_char][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index(Regexp.new('[A-Za-z_]'), index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 1))
        @index += 1
      else
        r0 = nil
      end

      node_cache[:alpha_char][start_index] = r0

      return r0
    end

    def _nt_numeric_char
      start_index = index
      if node_cache[:numeric_char].has_key?(index)
        cached = node_cache[:numeric_char][index]
        @index = cached.interval.end if cached
        return cached
      end

      if input.index(Regexp.new('[0-9]'), index) == index
        r0 = instantiate_node(Node::Boring,input, index...(index + 1))
        @index += 1
      else
        r0 = nil
      end

      node_cache[:numeric_char][start_index] = r0

      return r0
    end

    def _nt_alphanumeric_char
      start_index = index
      if node_cache[:alphanumeric_char].has_key?(index)
        cached = node_cache[:alphanumeric_char][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_alpha_char
      if r1
        r0 = r1
        r0.extend(Modules::Boring)
      else
        r2 = _nt_numeric_char
        if r2
          r0 = r2
          r0.extend(Modules::Boring)
        else
          self.index = i0
          r0 = nil
        end
      end

      node_cache[:alphanumeric_char][start_index] = r0

      return r0
    end

    def _nt_message_char
      start_index = index
      if node_cache[:message_char].has_key?(index)
        cached = node_cache[:message_char][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0 = index
      r1 = _nt_alpha_char
      if r1
        r0 = r1
        r0.extend(Modules::Boring)
      else
        r2 = _nt_numeric_char
        if r2
          r0 = r2
          r0.extend(Modules::Boring)
        else
          if input.index(Regexp.new('[?!]'), index) == index
            r3 = instantiate_node(SyntaxNode,input, index...(index + 1))
            @index += 1
          else
            r3 = nil
          end
          if r3
            r0 = r3
            r0.extend(Modules::Boring)
          else
            self.index = i0
            r0 = nil
          end
        end
      end

      node_cache[:message_char][start_index] = r0

      return r0
    end

    def _nt_number_message
      start_index = index
      if node_cache[:number_message].has_key?(index)
        cached = node_cache[:number_message][index]
        @index = cached.interval.end if cached
        return cached
      end

      s0, i0 = [], index
      loop do
        r1 = _nt_numeric_char
        if r1
          s0 << r1
        else
          break
        end
      end
      if s0.empty?
        self.index = i0
        r0 = nil
      else
        r0 = instantiate_node(Modules::Boring,input, i0...index, s0)
      end

      node_cache[:number_message][start_index] = r0

      return r0
    end

    module SymbolMessage0
      def plain_message
        elements[1]
      end
    end

    def _nt_symbol_message
      start_index = index
      if node_cache[:symbol_message].has_key?(index)
        cached = node_cache[:symbol_message][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      if input.index(":", index) == index
        r1 = instantiate_node(SyntaxNode,input, index...(index + 1))
        @index += 1
      else
        terminal_parse_failure(":")
        r1 = nil
      end
      s0 << r1
      if r1
        r2 = _nt_plain_message
        s0 << r2
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(SymbolMessage0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:symbol_message][start_index] = r0

      return r0
    end

    module OperatorMessage0
    end

    def _nt_operator_message
      start_index = index
      if node_cache[:operator_message].has_key?(index)
        cached = node_cache[:operator_message][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      i1 = index
      r2 = _nt_block_comment_open
      if r2
        r1 = nil
      else
        self.index = i1
        r1 = instantiate_node(SyntaxNode,input, index...index)
      end
      s0 << r1
      if r1
        s3, i3 = [], index
        loop do
          if input.index(Regexp.new('[\\+\\-\\*\\%\\=\\?\\!\\/]'), index) == index
            r4 = instantiate_node(SyntaxNode,input, index...(index + 1))
            @index += 1
          else
            r4 = nil
          end
          if r4
            s3 << r4
          else
            break
          end
        end
        if s3.empty?
          self.index = i3
          r3 = nil
        else
          r3 = instantiate_node(SyntaxNode,input, i3...index, s3)
        end
        s0 << r3
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(OperatorMessage0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:operator_message][start_index] = r0

      return r0
    end

    module PlainMessage0
      def alpha_char
        elements[0]
      end

    end

    def _nt_plain_message
      start_index = index
      if node_cache[:plain_message].has_key?(index)
        cached = node_cache[:plain_message][index]
        @index = cached.interval.end if cached
        return cached
      end

      i0, s0 = index, []
      r1 = _nt_alpha_char
      s0 << r1
      if r1
        s2, i2 = [], index
        loop do
          r3 = _nt_message_char
          if r3
            s2 << r3
          else
            break
          end
        end
        if s2.empty?
          self.index = i2
          r2 = nil
        else
          r2 = instantiate_node(SyntaxNode,input, i2...index, s2)
        end
        s0 << r2
      end
      if s0.last
        r0 = instantiate_node(SyntaxNode,input, i0...index, s0)
        r0.extend(PlainMessage0)
      else
        self.index = i0
        r0 = nil
      end

      node_cache[:plain_message][start_index] = r0

      return r0
    end

  end

  class GrammarParser < Treetop::Runtime::CompiledParser
    include Grammar
  end
 # grammar
end # module 