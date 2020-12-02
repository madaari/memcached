import pandas as pd
import plotly.express as px
import plotly.graph_objects as go
from plotly.subplots import make_subplots

df1 = pd.read_csv('./WIthoutNekara/memcached_coverage.txt')

trace1 = go.Scatter(
    x=df1['x'],
    y=df1['y'],
    name='Without Nekara'
)

df2 = pd.read_csv('./WithNekara/memcached_coverage_4lwp_with_nekara.txt')

trace2 = go.Scatter(
    x=df2['x'],
    y=df2['y'],
    name='With Nekara, Random'
)

df3 = pd.read_csv('./WithNekara/memcached_coverage_probabilistic_random_rolling.txt')

trace3 = go.Scatter(
    x=df3['x'],
    y=df3['y'],
    name='With Nekara, Random Probabilistic, 1000 steps, no extreme probability'
)

df4 = pd.read_csv('./WithNekara/memcached_coverage_probabilistic_random_500_steps.txt')

trace4 = go.Scatter(
    x=df4['x'],
    y=df4['y'],
    name='With Nekara, Random Probabilistic, 500 steps, no extreme probability'
)

df5 = pd.read_csv('./WithNekara/memcached_coverage_probabilistic_random_2000_steps.txt')

trace5 = go.Scatter(
    x=df5['x'],
    y=df5['y'],
    name='With Nekara, Random Probabilistic, 2000 steps, no extreme probability'
)


df6 = pd.read_csv('./WithNekara/memcached_coverage_probabilistic_random_rolling_300_steps_with_both_extremes.txt')

trace6 = go.Scatter(
    x=df6['x'],
    y=df6['y'],
    name='With Nekara, Random Probabilistic, 300 steps, with extreme probability'
)


df7 = pd.read_csv('./WithNekara/memcached_coverage_probabilistic_random_rolling_1000_steps_with_both_extremes.txt')

trace7 = go.Scatter(
    x=df7['x'],
    y=df7['y'],
    name='With Nekara, Random Probabilistic, 1000 steps, with extreme probability'
)

fig = make_subplots(1,1);
fig.update_layout(title_text="Memcached coverage. Hash of only slab only with just 10 deletes and 10 sets");

fig.add_trace(trace1);
fig.add_trace(trace2);
fig.add_trace(trace3);
fig.add_trace(trace4);
fig.add_trace(trace5);
fig.add_trace(trace6);
fig.add_trace(trace7);

fig.show()
