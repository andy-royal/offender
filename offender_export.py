import bpy
from bpy.types import Operator
from bpy.types import INFO_MT_file_export
from bpy_extras.io_utils import ExportHelper

# Give all objects a property to store the name of the class they'll become when exported
bpy.types.Object.offender_class_name = bpy.props.StringProperty(name="Class Name")

class offender_export_h(Operator, ExportHelper):
    """Tooltips my arse"""
    bl_idname = "export_mesh.offender_h"
    bl_label = "Export .h"

    # Needed by ExportHelper
    filename_ext = ".h"

    def execute(self, context):
        filepath = self.filepath
        print("Writing mesh to %s\n" % filepath)
        f = open(filepath, 'w', encoding='utf-8')

        obj = context.object
        class_name = obj.offender_class_name
        f.write("#pragma once\n"
                "#include \"stdafx.h\"\n"
                "\n"
                "namespace Offender {\n"
                "\n"
                "    class " + class_name + " : public Object {\n"
                "        private:\n"
                "            GLuint          m_vertexBuffer;\n"
                "            GLuint          m_indexBuffer;\n"
                "            void            InitGeometry();\n"
                "        public:\n"
                "            " + class_name + "(World*, ObjPos, ObjVec, ObjQuat);\n"
                "            GLboolean       Draw();\n"
                "    };\n"
                "\n"
                "} // namespace Offender\n")

        f.close()
        return {'FINISHED'}

class offender_export_cpp(Operator, ExportHelper):
    """Tooltips my arse"""
    bl_idname = "export_mesh.offender_cpp"
    bl_label = "Export .cpp"

    # Needed by ExportHelper
    filename_ext = ".cpp"

    def execute(self, context):
        filepath = self.filepath
        print("Writing mesh to %s\n" % filepath)
        f = open(filepath, 'w', encoding='utf-8')
        
        obj = context.object
        class_name = obj.offender_class_name
        mesh = obj.data
        verts = mesh.vertices
        #uv_layer = mesh.uv_layers.active.data
        
        polygons = mesh.tessfaces
        mesh.update(calc_tessface=True)

        # Need to check number of indices early on so index array can be sized
        index_array_size = 0
        for poly in polygons:
            index_array_size += len(poly.vertices)

        f.write("#pragma once\n"
                "#include \"stdafx.h\"\n"
                "\n"
                "namespace Offender {\n"
                "\n"
                "    " + class_name + "::" + class_name + "(World* l_world, ObjPos l_pos, ObjVec l_vec, ObjQuat l_orient) :\n"
                "                        Object(l_world, l_pos, l_vec, l_orient) {\n"
                "        InitGeometry();\n"
                "    }\n"
                "\n"
                #"    // Vertices = " + len(mesh.vertices) + "\n"
                "    // Object type = " + obj.type + "\n"
                "    // Vertices = " + str(len(verts)) + "\n"
                "    // Polygons = " + str(len(polygons)) + "\n")
        #for poly in mesh.polygons:
            #f.write("Polygon index: %d, length: %d\n" % (poly.index, poly.loop_total))

            # range is used here to show how the polygons reference loops,
            # for convenience 'poly.loop_indices' can be used instead.
            #for loop_index in range(poly.loop_start, poly.loop_start + poly.loop_total):
                #f.write("    Vertex: %d\n" % mesh.loops[loop_index].vertex_index)
                #f.write("    UV: %r\n" % uv_layer[loop_index].uv)
        f.write("    void " + class_name + "::InitGeometry() {\n"
                "        SetCollisionRadius(sqrt(0.5f));\n"
                "\n"
                "        ObjectVertex vertices[" + str(len(verts)) + "];\n"
                "        GLuint indices[" + str(index_array_size) + "];\n"
                "\n")
        max_x = 0
        max_y = 0
        max_z = 0
        for vert in verts:
            # Swap Y and Z
            f.write("        vertices[" + str(vert.index) + "].position[0] = " + str(vert.co[0]) + "f;\n")
            f.write("        vertices[" + str(vert.index) + "].position[1] = " + str(vert.co[2]) + "f;\n")
            f.write("        vertices[" + str(vert.index) + "].position[2] = " + str(vert.co[1]) + "f;\n")
            f.write("        vertices[" + str(vert.index) + "].normal[0] = " + str(vert.normal[0]) + "f;\n")
            f.write("        vertices[" + str(vert.index) + "].normal[1] = " + str(vert.normal[2]) + "f;\n")
            f.write("        vertices[" + str(vert.index) + "].normal[2] = " + str(vert.normal[1]) + "f;\n")
        f.write("\n")
        index = 0
        for poly in polygons:
            if len(poly.vertices) == 3:
                for vertex in poly.vertices:
                    f.write("        indices[" + str(index) +"] = " + str(vertex) + ";\n")
                    index += 1
            elif len(poly.vertices) == 4:
                f.write("        indices[" + str(index) +"] = " + str(poly.vertices[0]) + ";\n")
                f.write("        indices[" + str(index+1) +"] = " + str(poly.vertices[1]) + ";\n")
                f.write("        indices[" + str(index+2) +"] = " + str(poly.vertices[3]) + ";\n")
                f.write("        indices[" + str(index+3) +"] = " + str(poly.vertices[2]) + ";\n")               
                index += 4
                
        f.write("\n")
        f.write("        glGenBuffers(1, &m_vertexBuffer);\n"
                "        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);\n"
                "        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);\n"
                "        glBindBuffer(GL_ARRAY_BUFFER, 0);\n"
                "\n"
                "        glGenBuffers(1, &m_indexBuffer);\n"
                "        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);\n"
                "        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);\n"
                "        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);\n"
                "    }\n"
                "\n"
                "    GLboolean " + class_name + "::Draw() {\n"
                "        // Common setup for default program\n"
                "        DrawSetup();\n"
                "\n"
                "        // Load the vertex data\n"
                "        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);\n"
                "        glVertexAttribPointer (POSITION_HANDLE, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), reinterpret_cast<void*>(offsetof(vertexStruct,position)));\n"
                "        glEnableVertexAttribArray (POSITION_HANDLE);\n"
                "        glVertexAttribPointer (NORMAL_HANDLE, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), reinterpret_cast<void*>(offsetof(vertexStruct,normal)));\n"
                "        glEnableVertexAttribArray (NORMAL_HANDLE);\n"
                "\n"
                "        // Draw\n"
                "        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);\n")

        index = 0
        for poly in polygons:
                #"        glDrawRangeElements (GL_TRIANGLE_STRIP, 0, 5, 5, GL_UNSIGNED_INT, reinterpret_cast<void*>(0));\n"
                #"        glDrawRangeElements (GL_TRIANGLE_STRIP, 0, 5, 5, GL_UNSIGNED_INT, reinterpret_cast<void*>(5*sizeof(GLuint)));\n"
                #"        glDrawRangeElements (GL_TRIANGLE_STRIP, 0, 5, 4, GL_UNSIGNED_INT, reinterpret_cast<void*>(10*sizeof(GLuint)));\n"
            count = len(poly.vertices)
            f.write("        glDrawRangeElements (GL_TRIANGLE_STRIP, 0, " + str(len(verts)-1) + ", " + str(count) + ", GL_UNSIGNED_INT, reinterpret_cast<void*>(" + str(index) + "*sizeof(GLuint)));\n")
            index += count
                
        f.write("\n"
                "        // Detach buffers\n"
                "        glBindBuffer(GL_ARRAY_BUFFER, 0);\n"
                "        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);\n"
                "\n"
                "        return GL_TRUE;\n"
                "    }\n"
                "\n"
                "} // namespace Offender\n")

        f.close()
        return {'FINISHED'}

class offender_export(Operator):
    """Tooltips my arse"""
    bl_idname = "export_mesh.offender"
    bl_label = "Export"
    
    def execute(self, context):
        #offender_export_h('INVOKE_DEFAULT')
        #offender_export_cpp('INVOKE_DEFAULT')
        bpy.ops.export_mesh.offender_h('INVOKE_DEFAULT')
        bpy.ops.export_mesh.offender_cpp('INVOKE_DEFAULT')
        return {'FINISHED'}
    
class OffenderExportPanel(bpy.types.Panel):
    bl_idname = "OBJECT_PT_offender"
    bl_label = "Offender"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "object"
    bl_options = {'DEFAULT_CLOSED'}

    def draw(self, context):
        layout = self.layout

        obj = context.object
        row = layout.row()
        row.prop(obj, "offender_class_name")
        #row = layout.row()
        row = layout.row()
        row.operator("export_mesh.offender_h")
        row.operator("export_mesh.offender_cpp")
        row = layout.row()
        row.operator("export_mesh.offender")

#def menu_func(self, context):
#    self.layout.operator(offender_export.bl_idname, text="Offender")
    
def register():
    bpy.utils.register_module(__name__)
    #bpy.types.INFO_MT_file_export.append(menu_func)
    
def unregister():
    bpy.utils.unregister_module(__name__)
    #bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()