# /*!
#  * Copyright 2019 by Panyi Inc
#  * \file test_util.py
#  * \brief This is a set of python utility functions used by other applications.
#  *  Author: Steven Wang, swang@panyi.ai.
#  */
import numpy as np
import tensorflow as tf

def random_floats(low, high, size):
	return [np.random.uniform(low, high) for _ in xrange(size)]

def pad_with(vector, pad_width, iaxis, kwargs):
     pad_value = kwargs.get('padder', 10)
     if (pad_width[0]>0):
	     vector[:pad_width[0]] = pad_value
     if (pad_width[1]>0):
	     vector[-pad_width[1]:] = pad_value
     return vector

def save_data_3d(data, filename):
	with open(filename, 'w') as outfile:
	    # I'm writing a header here just for the sake of readability
	    # Any line starting with "#" will be ignored by numpy.loadtxt
	    outfile.write('# Array shape: {0}\n'.format(data.shape))
	
	    # Iterating through a ndimensional array produces slices along
	    # the last axis. This is equivalent to data[i,:,:] in this case
	    for data_slice in data:	
	        # The formatting string indicates that I'm writing out
	        # the values in left-justified columns 7 characters in width
	        # with 8 decimal places.  
	        np.savetxt(outfile, data_slice, fmt='%-10.8f')	
	        # Writing out a break to indicate different slices...
	        outfile.write('# New slice\n')    
	        
def save_data(data, shape, filename):
	dim = len(shape)
	if (dim < 2):
		np.savetxt(filename, data, fmt='%-7.8f')
	elif (dim == 3):
		save_data_3d(data, filename)
	elif (dim > 3):
	    merge_cnt = dim - 2
	    cnt = 1
	    for i in range(merge_cnt):
	    	cnt = cnt*shape[i]
	    d_data=data.reshape(cnt, shape[dim-2], shape[dim -1])
	    save_data_3d(d_data, filename)

def load_data_3d(filename, shape):
	t_data = np.loadtxt(filename)
	s_data = t_data.reshape(shape)
	return s_data

def load_data(filename, shape):
	dim = len(shape)
	if (dim <= 2):
		t_data = np.loadtxt(filename)
	elif (dim == 3):
		t_data=load_data_3d(filename,shape)
	elif (dim > 3):
		s_data=load_data_3d(filename,shape)
		t_data=s_data.reshape(shape)
	return t_data

# example: gen_data([(1,1),(1,1),(0,32)], (28,28,512), 0)
# outputs: (30,30,544) with 0 padding
def gen_data(pad_shape, shape, count, padder_v):
	t_data = np.random.rand(count).reshape(shape)
# 	t_data = random_floats(-1.0, 1.0, count).reshape(shape)
	t_data = np.pad(t_data, pad_shape, pad_with, padder = padder_v)
	return t_data

def tf_conv_op(inputs, filters):
	op = tf.nn.conv2d(inputs, filters, strides=[1, 1, 1, 1], padding='SAME')
	return op
			
def tf_conv(inputs, filters):
	op = tf.nn.conv2d(inputs, filters, strides=[1, 1, 1, 1], padding='SAME')
	init = tf.initialize_all_variables()
	with tf.Session() as sess:
		sess.run(init)
    	result = sess.run(op)
    	#print(result)
	return result

def verify():
	tf_res = load_data('result.dat', (1,28,28,512))
	w=load_data('weights.dat', (3,3,512,512))
	d=load_data('image_no_pad.dat', (28,28,512))
	d=d.reshape(1,28,28,512)
	w_pint=load_data('weights_8_512_3_3_64.dat',(8,512,3,3,64))
	d_pint=load_data('image_with_padding.dat', (30,30,544))[:,:,:512].reshape(1,30,30,512)
	print("original weights")
	print(d.transpose(3,2,0,1)[0,0,:5,:5])
	print("transformed weights")
	print(w_pint.transpose(1,4,0,2,3)[0,0,0,:5,:5])
	print("original data")
	print(d.transpose(0,3,1,2)[0,0,:5,:5])
	print("transformed data")
	print(d_pint[0,:5,:5,0])
# 	op = tf.nn.conv2d(d_pint, w, strides=[1, 1, 1, 1], padding='SAME')
# 	op = tf.nn.conv2d(d_pint, w, strides=[1, 1, 1, 1], padding='SAME')
	op = tf.nn.conv2d(d, w, strides=[1, 1, 1, 1], padding='VALID')
	init = tf.initialize_all_variables()
	sess=tf.Session()
	sess.run(init)
	res = sess.run(op)
	print("result:..... shape {}".format(res.shape))
	print(res.transpose(0,3,1,2)[0,0,:5,:5])
	
	err=np.square(tf_res-res).mean()
	print("MSE???:, err = {}".format(err))