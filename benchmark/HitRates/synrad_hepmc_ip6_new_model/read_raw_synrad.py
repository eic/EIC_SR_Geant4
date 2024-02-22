# Import libraries
import os
import time
import glob
import math
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl

time_start = time.perf_counter()

#---------------------------------------------#
#---------------------------------------------#
path = '../synrad_data_ip6_new_model/data_ip'
# path = '../synrad_data_ip6_new_model/data_facet'
# path = '../synrad_data_ip6_new_model/data_facet_ene'
Z_min = -67.0 # [cm]
Z_max = +68.69231 # [cm]
E_min = 30 # [eV]
E_max = 1e12 # [eV]
outdir = './output_data'
outfile = 'output.csv'
#---------------------------------------------#
#---------------------------------------------#
# Create an output directory
try: 
    os.makedirs(outdir, exist_ok = True) 
    print("\n--> directory '%s' created successfully" % outdir) 
except OSError as error: 
    print("\n--> directory '%s' can not be created" % outdir) 

# Get CSV files list from an input directory
csv_files = glob.glob(path + "/*.csv")

print("\n--> csv files: ")
print(csv_files)

# Read each CSV file into DataFrame
# This creates a list of dataframes
df_list = (pd.read_csv(file) for file in csv_files)

time_ini = time.perf_counter()
# Concatenate all DataFrames
all_df = pd.concat(df_list, ignore_index=True)
print('\n--> spent time to load all csv files: ' + str("{:.2f}".format(time.perf_counter() - time_ini)) + 'sec')
print(all_df)

time_ini = time.perf_counter()
# Store only within a given range
all_df_range = all_df[(all_df['Pos_Z_[cm]'] >= Z_min) & (all_df['Pos_Z_[cm]'] <= Z_max) & (all_df['Energy_[eV]'] >= E_min) & (all_df['Energy_[eV]'] <= E_max)].copy()
print('\n--> spent time to copy dataframe: ' + str("{:.2f}".format(time.perf_counter() - time_ini)) + 'sec')
print(all_df_range)

all_df_range['Dir_Norm'] = all_df_range['Dir_X'].pow(2) + all_df_range['Dir_Y'].pow(2) + all_df_range['Dir_Z'].pow(2)
all_df_range['Dir_Norm'] = all_df_range['Dir_Norm'].pow(1./2.)
all_df_range['Mom_X_[eV]'] = all_df_range['Dir_X']*all_df_range['Energy_[eV]']
all_df_range['Mom_Y_[eV]'] = all_df_range['Dir_Y']*all_df_range['Energy_[eV]']
all_df_range['Mom_Z_[eV]'] = all_df_range['Dir_Z']*all_df_range['Energy_[eV]']
print(all_df_range)

# Weighted photon spectrum
f = plt.figure(figsize=(15,8))
all_df_range['Energy_[eV]'].plot(kind="hist", weights=all_df_range['Flux_[photon/s]'], logy=True, bins=200)

# Save the figure in the PNG format
fname = outdir + "/output0.png"
plt.savefig(fname, dpi=400)
print('\n--> saved figure to: ' + fname)

# Draw 1D histograms for all columns
i = 1
f = plt.figure(figsize=(15,8))
for col in all_df_range.columns:
	dff = all_df_range[col].dropna().unique()
	ax = f.add_subplot(4,5,i)
	ax.set_title(col)
	ax.hist(dff,bins=10)
	i+=1

plt.subplots_adjust(wspace=0.4)
plt.subplots_adjust(hspace=0.8)

# Save the figure in the PNG format
fname = outdir + "/output1.png"
plt.savefig(fname, dpi=400)
print('\n--> saved figure to: ' + fname)

# Draw 2D histograms
#-- 1
fig, axs = plt.subplots(nrows=2, ncols=3, figsize=(15, 8))
axs[0, 0].set_title('Y [cm] vs X [cm]')
hh1 = axs[0, 0].hist2d(all_df_range['Pos_X_[cm]'],all_df_range['Pos_Y_[cm]'],bins=50,norm=mpl.colors.LogNorm())
fig.colorbar(hh1[3], ax=axs[0, 0])
#-- 2
axs[0, 1].set_title('Y [cm] vs Z [cm]')
hh2 = axs[0, 1].hist2d(all_df_range['Pos_Z_[cm]'],all_df_range['Pos_Y_[cm]'],bins=50,norm=mpl.colors.LogNorm())
fig.colorbar(hh2[3], ax=axs[0, 1])
#-- 3
axs[0, 2].set_title('X [cm] vs Z [cm]')
hh3 = axs[0, 2].hist2d(all_df_range['Pos_Z_[cm]'],all_df_range['Pos_X_[cm]'],bins=50,norm=mpl.colors.LogNorm())
fig.colorbar(hh3[3], ax=axs[0, 2])
#-- 4
axs[1, 0].set_title('Flux [ph/s] vs Z [cm]')
hh4 = axs[1, 0].hist2d(all_df_range['Pos_Z_[cm]'],all_df_range['Flux_[photon/s]'],bins=50,norm=mpl.colors.LogNorm())
fig.colorbar(hh4[3], ax=axs[1, 0])
#-- 5
axs[1, 1].set_title('Flux [ph/s] vs Energy [eV]')
hh5 = axs[1, 1].hist2d(all_df_range['Energy_[eV]'],all_df_range['Flux_[photon/s]'],bins=1000,norm=mpl.colors.LogNorm())
fig.colorbar(hh5[3], ax=axs[1, 1])
#-- 6
axs[1, 2].set_title('Power [W] vs Z [cm]')
hh6 = axs[1, 2].hist2d(all_df_range['Pos_Z_[cm]'],all_df_range['Power_[W]'],bins=50,norm=mpl.colors.LogNorm())
fig.colorbar(hh6[3], ax=axs[1, 2])

plt.subplots_adjust(wspace=0.2)
plt.subplots_adjust(hspace=0.3)

# Save the figure in the PNG format
fname = outdir + '/output2.png'
plt.savefig(fname, dpi=400)
print('\n--> saved figure to: ' + fname)

# Plot figures
# plt.show()

# Write data to file
print('\n--> writing to file ...')
time_ini = time.perf_counter()
header = ['Pos_X_[cm]','Pos_Y_[cm]','Pos_Z_[cm]','Mom_X_[eV]','Mom_Y_[eV]','Mom_Z_[eV]','Flux_[photon/s]']
all_df_range.to_csv(outdir + '/' + outfile, columns = header,index=False)
print('\n--> spent time to load all csv files: ' + str("{:.2f}".format(time.perf_counter() - time_ini)) + 'sec')

print('\n--> running time: ' + str("{:.2f}".format(time.perf_counter() - time_start)) + 'sec')
