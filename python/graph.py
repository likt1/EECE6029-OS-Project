from bokeh.plotting import figure, show, output_notebook, output_file
from bokeh.models import ColumnDataSource, Range1d
from bokeh.models.tools import HoverTool
from bokeh.charts import Bar, output_file, show

import pandas as ps
import sys

#data = ps.read_csv(sys.argv[1])
data = ps.read_csv('../output/results.csv')
G1 = Bar(data, 'ID', values='Turn-Around', title='Turn-Around Time by ID')
G2 = Bar(data, 'ID', values='Waiting',     title='Waiting Time by ID')

'''
hover=HoverTool(tooltips="Process #@Process<br>\
TAT: @Turn-Around<br>\
WT:  @Waiting")
G.add_tools(hover)
'''

output_file('../output/turn-around-time.html')
show(G1)
output_file('../output/waiting-time.html')
show(G2)