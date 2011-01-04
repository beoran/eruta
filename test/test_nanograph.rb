require 'test_helper.rb'

require 'nanograph'

assert { Nanograph }
assert { Nanograph::Graph }
assert { Nanograph::Node }
assert { Nanograph::Edge }

graph     = Nanograph.graph(fontsize: 10, splines: true, overlap: false,
                            rankdir: "LR")
assert { graph } 

graph.nodeattr = { fontsize: 10, width:0, height:0, shape: :box, 
                   style: :rounded }

assert { graph.nodeattr } 

nod1 = graph.node(nil , "Hello, hello") >> "'world'" >> '!'
assert { nod1 } 

nod2 = graph.node('node_1') >> 'world 2'
assert { nod2 } 

nod3 = graph.node('!')
assert { nod3 } 

nod4 = graph.node('world 2')
assert { nod4 } 


assert { graph.to_png('test_graph.png') } 


graph.display

