#include "ModMatrix.h"

ModConnection::ModConnection(ModSource* src, ModDestination* dst, float d) : source(src), destination(dst), depth(d) {}

void ModMatrix::removeConnection(ModConnection* connection) {
	connections.remove_if([connection](const ModConnection& c) { return &c == connection; });
}
