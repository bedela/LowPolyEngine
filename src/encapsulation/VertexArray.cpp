//
// Created by adrien on 20/02/20.
//

#include "encapsulation/VertexArray.h"

VertexArray::VertexArray()
{
}

void VertexArray::addVertexBuffer(const void *data, unsigned int size, const Layout &layout)
{
    unsigned int offset = 0;

    _vBuffers.emplace_back(data, size);

    for (const auto &element : layout._elements)
    {
        glEnableVertexAttribArray(element.location);
        glVertexAttribPointer(element.location, element.count, element.type, element.normalized, layout._size, &offset);
        offset += element.count * sizeof(element.type);
    }
}