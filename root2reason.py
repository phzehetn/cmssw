import pandas as pd
import numpy as np
import uproot

df = pd.DataFrame()


file_EM = uproot.open("ticlTrackstersEM.root")
file_HAD = uproot.open("ticlTrackstersHAD.root")
file_Trk = uproot.open("ticlTrackstersTrk.root")
file_TrkEM = uproot.open("ticlTrackstersTrkEM.root")
file_Merge = uproot.open("ticlTrackstersMerge.root")
file_Sim = uproot.open("ticlTrackstersSim.root")

tree_EM = file_EM["tracksters;1"]
tree_HAD = file_HAD["tracksters;1"]
tree_Trk = file_Trk["tracksters;1"]
tree_TrkEM = file_TrkEM["tracksters;1"]
tree_Merge = file_Merge["tracksters;1"]
tree_Sim = file_Sim["tracksters;1"]

keys_EM = tree_EM.keys()
print(type(keys_EM))
