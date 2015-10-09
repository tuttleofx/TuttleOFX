%module tuttle

%module(directors="1", threads="1") tuttle

%include <tuttle/host/version.hpp>

%include "global.i"
%include "Core.i"
%include "ImageEffectNode.i"
%include "InputBufferWrapper.i"
%include "attribute/allParams.i"
%include "Graph.i"
%include "graph/ProcessGraph.i"
%include "ThreadEnv.i"
%include "Node.i"
%include "OverlayInteract.i"
%include "io.i"
%include "thumbnail/ThumbnailDiskCache.i"
%include "Callback.i"
%include <tuttle/common/utils/Formatter.i>
