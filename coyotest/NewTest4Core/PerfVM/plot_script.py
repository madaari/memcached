import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots

df1 = pd.read_csv('./memcached_coverage_slab_only_vm_without_nekara.txt')

trace1 = go.Scatter(
    x=df1['x'],
    y=df1['y'],
    name='Without Nekara. Using 2 logical cores'
)

df2 = pd.read_csv('./memcached_coverage_slab_only_4core_without_nekara.txt')

trace2 = go.Scatter(
    x=df2['x'],
    y=df2['y'],
    name='Without Nekara. Using 4 logical cores'
)


fig = make_subplots(1,1);
fig.update_layout(title_text="Memcached coverage. Hash of only slab only with just 10 deletes and 10 sets.");

fig.add_trace(trace1);
fig.add_trace(trace2);

fig.show()
