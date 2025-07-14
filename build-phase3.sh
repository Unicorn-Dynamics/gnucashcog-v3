#!/bin/bash

# Build script for Phase 3: Neural-Symbolic Synthesis via Custom ggml Kernels
# This script builds the Phase 3 neural-symbolic demonstration without external dependencies

echo "======================================================================="
echo "Phase 3: Neural-Symbolic Synthesis via Custom ggml Kernels Build"
echo "======================================================================="

# Set up build environment
export CC=gcc
export CXX=g++
export CFLAGS="-O2 -g -DHAVE_CONFIG_H -I. -I./libgnucash/engine"
export CXXFLAGS="-O2 -g -std=c++11 -DHAVE_CONFIG_H -I. -I./libgnucash/engine"

# Create build directory
mkdir -p build-phase3
cd build-phase3

echo "Building Phase 3 neural-symbolic kernel components..."

# Compile the neural-symbolic kernels implementation
echo "Compiling gnc-neural-symbolic-kernels.cpp..."
$CXX $CXXFLAGS -c ../libgnucash/engine/gnc-neural-symbolic-kernels.cpp \
    -o gnc-neural-symbolic-kernels.o

if [ $? -ne 0 ]; then
    echo "Error: Failed to compile gnc-neural-symbolic-kernels.cpp"
    exit 1
fi

# Compile tensor network implementation (dependency)
echo "Compiling gnc-tensor-network.cpp..."
$CXX $CXXFLAGS -c ../libgnucash/engine/gnc-tensor-network.cpp \
    -o gnc-tensor-network.o

# Create minimal stubs for dependencies
echo "Creating minimal stubs for missing dependencies..."

cat > gnc-cognitive-stubs.cpp << 'EOF'
#include <glib.h>

// Minimal stubs for missing cognitive accounting functions
extern "C" {

gboolean gnc_cognitive_accounting_init(void) {
    g_message("Cognitive accounting stub initialized");
    return TRUE;
}

void gnc_cognitive_accounting_shutdown(void) {
    g_message("Cognitive accounting stub shutdown");
}

// Tensor data stub implementation
typedef struct {
    gfloat *data;
    gsize *shape;
    gsize n_dims;
    gsize total_size;
    gchar *name;
} GncTensorData;

GncTensorData* gnc_tensor_data_create(const gchar *name, gsize n_dims, gsize *shape) {
    GncTensorData *tensor = g_new0(GncTensorData, 1);
    tensor->name = g_strdup(name);
    tensor->n_dims = n_dims;
    tensor->shape = g_new(gsize, n_dims);
    
    tensor->total_size = 1;
    for (gsize i = 0; i < n_dims; i++) {
        tensor->shape[i] = shape[i];
        tensor->total_size *= shape[i];
    }
    
    tensor->data = g_new0(gfloat, tensor->total_size);
    return tensor;
}

void gnc_tensor_data_destroy(GncTensorData *tensor) {
    if (!tensor) return;
    g_free(tensor->data);
    g_free(tensor->shape);
    g_free(tensor->name);
    g_free(tensor);
}

gboolean gnc_tensor_network_init(void) {
    g_message("Tensor network stub initialized");
    return TRUE;
}

void gnc_tensor_network_shutdown(void) {
    g_message("Tensor network stub shutdown");
}

} // extern "C"
EOF

echo "Compiling cognitive stubs..."
$CXX $CXXFLAGS -c gnc-cognitive-stubs.cpp -o gnc-cognitive-stubs.o

# Compile the demonstration program
echo "Compiling neural-symbolic-synthesis-demo.cpp..."
$CXX $CXXFLAGS -c ../neural-symbolic-synthesis-demo.cpp \
    -o neural-symbolic-synthesis-demo.o

if [ $? -ne 0 ]; then
    echo "Error: Failed to compile neural-symbolic-synthesis-demo.cpp"
    exit 1
fi

# Link the demonstration executable
echo "Linking neural-symbolic synthesis demonstration..."
$CXX -o neural-symbolic-synthesis-demo \
    neural-symbolic-synthesis-demo.o \
    gnc-neural-symbolic-kernels.o \
    gnc-tensor-network.o \
    gnc-cognitive-stubs.o \
    -lm -lstdc++

if [ $? -ne 0 ]; then
    echo "Error: Failed to link neural-symbolic synthesis demonstration"
    exit 1
fi

echo "✓ Phase 3 neural-symbolic synthesis demonstration built successfully!"
echo ""
echo "Executable: build-phase3/neural-symbolic-synthesis-demo"
echo ""
echo "To run the demonstration:"
echo "  cd build-phase3"
echo "  ./neural-symbolic-synthesis-demo"
echo ""
echo "======================================================================="