from bokeh.plotting import figure, show, output_notebook, output_file
from bokeh.models import ColumnDataSource, Range1d
from bokeh.models.tools import HoverTool
from datetime import datetime
from bokeh.charts import Bar

import pandas as ps
import sys

data = ps.read_csv(sys.argv[1])

# might have to convert columns to numbers here...
data['ID'] = data['ID'].fillna(-1)
data['Process'] = data['Process'].fillna(-1)

data.sort('Process')

data['ID_s'] = data['ID'].astype(int).astype(str)
data['Process_s']=data['Process'].astype(int).astype(str)


G=figure(title='Gantt Chart, Scheduler',width=800,height=400, y_range=Range1d(data.Process.min()-.25, data.Process.max()+.25), x_range=Range1d(data.Start.min(),data.End.max()), tools='save')

hover=HoverTool(tooltips="Process #@Process<br>\
UID: @ID<br>\
Start: @Start<br>\
End: @End")
G.add_tools(hover)

data['idx_b']=data.Process-0.2
data['idx_t']=data.Process+0.2
CDS=ColumnDataSource(data)
G.quad(left='Start', right='End', bottom='idx_b', top='idx_t',source=CDS)
#G.quad(left='Start', right='End', bottom='idx_b', top='idx_t',source=CDS,color='Color')

output_file('../output/gantt.html')
show(G)