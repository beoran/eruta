    
#     Effort Applied = ab(KLOC)bb [ man-months ]
#     Development Time = cb(Effort Applied)db [months]
#     People required = Effort Applied / Development Time [count]
# 
# The coefficients ab, bb, cb and db are given in the following table.
# Software project  ab  bb  cb  db
# Organic   2.4   1.05  2.5   0.38
# Semi-detached   3.0   1.12  2.5   0.35
# Embedded  3.6   1.20  2.5   0.32    
    
class Cocomo
  ORGANIC       = { :ab => 2.40, :bb => 1.05, :cb => 2.50, :db => 0.38 }  
  SEMI_DETACHED = { :ab => 3.00, :bb => 1.12, :cb => 2.50, :db => 0.35 }
  EMBEDDED      = { :ab => 3.60, :bb => 1.20, :cb => 2.50, :db => 0.32 }
  
  BJMEY         = { :ab => 0.20, :bb => 1.05, :cb => 2.50, :db => 0.38 }
  
  def initialize(params) 
    @params = params
  end
  
  def ab
    @params[:ab] 
  end
  
  def bb
    @params[:bb] 
  end
  
  def cb
    @params[:cb] 
  end
   
  def db
    @params[:db] 
  end
   
   
  def effort2klock(klock) 
    ab * (klock ** bb)  
  end
  
  def effort2time(effort)
    cb * (effort**db) 
  end
  
  def klock2effort(klock)
    (klock.to_f / ab) ** (1.0 / bb) 
  end
  
  def time2effort(tm)
    (tm / cb) ** (1.0 / db) 
  end  
end

MAN_MONTH_DAYS = 20.0

model = Cocomo.new(Cocomo::ORGANIC)
p model.klock2effort(1) * 1000  / MAN_MONTH_DAYS
# 
# model = Cocomo.new(Cocomo::BJMEY)
# p model.klock2effort(1 / MAN_MONTH_DAYS) * 1000
model = Cocomo.new(Cocomo::EMBEDDED)
p model.klock2effort(1) * 1000  / MAN_MONTH_DAYS





