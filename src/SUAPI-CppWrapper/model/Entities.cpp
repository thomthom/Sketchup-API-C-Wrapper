//
//  Entities.cpp
//
// Sketchup C++ Wrapper for C API
// MIT License
//
// Copyright (c) 2017 Tom Kaneko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <cassert>

#include "SUAPI-CppWrapper/model/Entities.hpp"

#include "SUAPI-CppWrapper/model/GeometryInput.hpp"
#include "SUAPI-CppWrapper/model/Vertex.hpp"
#include "SUAPI-CppWrapper/model/Loop.hpp"
#include "SUAPI-CppWrapper/Transformation.hpp"
#include "SUAPI-CppWrapper/model/ComponentDefinition.hpp"
#include "SUAPI-CppWrapper/model/ComponentInstance.hpp"
#include "SUAPI-CppWrapper/model/Group.hpp"
#include "SUAPI-CppWrapper/model/Face.hpp"
#include "SUAPI-CppWrapper/model/Edge.hpp"
#include "SUAPI-CppWrapper/model/Model.hpp"

namespace CW {


Entities::Entities(SUEntitiesRef entities/*, Model* model*/):
  m_entities(entities)/*,
  m_model(model)*/
{}

Entities::Entities():
  m_entities(SU_INVALID)/*,
  m_model(nullptr)*/
{}


std::vector<Face> Entities::faces() const {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::faces(): Entities is null");
  }
  size_t count = 0;
  SUResult res = SUEntitiesGetNumFaces(m_entities, &count);
  assert(res == SU_ERROR_NONE);
  if (count == 0) {
    return std::vector<Face>(0);
  }
  std::vector<SUFaceRef> face_refs(count, SU_INVALID);
  res = SUEntitiesGetFaces(m_entities, count, face_refs.data(), &count);
  assert(res == SU_ERROR_NONE);
  std::vector<Face> faces(count);
  std::transform(face_refs.begin(), face_refs.end(), faces.begin(),
  [](const SUFaceRef& value) {
    return Face(value);
  });
  return faces;
}


std::vector<Edge> Entities::edges(bool stray_only) const {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::edges(): Entities is null");
  }
  size_t count = 0;
  SUResult res = SUEntitiesGetNumEdges(m_entities, stray_only, &count);
  assert(res == SU_ERROR_NONE);
  if (count == 0) {
    return std::vector<Edge>(0);
  }
  std::vector<SUEdgeRef> edge_refs(count, SU_INVALID);
  res = SUEntitiesGetEdges(m_entities, stray_only, count, edge_refs.data(), &count);
  assert(res == SU_ERROR_NONE);
  std::vector<Edge> edges(count);
  std::transform(edge_refs.begin(), edge_refs.end(), edges.begin(),
  [](const SUEdgeRef& value) {
    return Edge(value);
  });
  return edges;
}


std::vector<ComponentInstance> Entities::instances() const {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::instances(): Entities is null");
  }
  size_t count = 0;
  SUResult res = SUEntitiesGetNumInstances(m_entities, &count);
  assert(res == SU_ERROR_NONE);
  if (count == 0) {
    return std::vector<ComponentInstance>{};
  }
  std::vector<SUComponentInstanceRef> instance_refs(count, SU_INVALID);
  res = SUEntitiesGetInstances(m_entities, count, instance_refs.data(), &count);
  assert(res == SU_ERROR_NONE);
  std::vector<ComponentInstance> instances(count);
  std::transform(instance_refs.begin(), instance_refs.end(), instances.begin(),
  [](const SUComponentInstanceRef& value) {
    return ComponentInstance(value);
  });;
  return instances;
}


std::vector<Group> Entities::groups() const {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::groups(): Entities is null");
  }
  size_t count = 0;
  SUResult res = SUEntitiesGetNumGroups(m_entities, &count);
  assert(res == SU_ERROR_NONE);
  if (count == 0) {
    return std::vector<Group>(0);
  }
  std::vector<SUGroupRef> group_refs(count);
  res = SUEntitiesGetGroups(m_entities, count, &group_refs[0], &count);
  assert(res == SU_ERROR_NONE);
  std::vector<Group> groups;
  groups.reserve(count);
  for (size_t i=0; i < group_refs.size(); ++i) {
    groups.push_back(Group(group_refs[i], true));
  }
  return groups;
}


BoundingBox3D Entities::bounding_box() const {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::groups(): Entities is null");
  }
  SUBoundingBox3D box = SU_INVALID;
  SUResult res = SUEntitiesGetBoundingBox(m_entities, &box);
  assert(res == SU_ERROR_NONE);
  return BoundingBox3D(box);
}


size_t Entities::size() const {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::size(): Entities is null");
  }
  size_t total_count = 0;
  size_t count = 0;
  SUResult res = SUEntitiesGetNumFaces(m_entities, &count);
  assert(res == SU_ERROR_NONE);
  total_count += count;
  count = 0;
  res = SUEntitiesGetNumEdges(m_entities, true, &count);
  assert(res == SU_ERROR_NONE);
  total_count += count;
  count = 0;
  res = SUEntitiesGetNumInstances(m_entities, &count);
  assert(res == SU_ERROR_NONE);
  total_count += count;
  count = 0;
  res = SUEntitiesGetNumGroups(m_entities, &count);
  assert(res == SU_ERROR_NONE);
  total_count += count;
  return total_count;
}


void Entities::add(const Entities& other) {
  if (!SUIsValid(m_entities) || !SUIsValid(other.m_entities)) {
    throw std::logic_error("CW::Entities::add(): Entities is null");
  }
  GeometryInput geom_input;
  geom_input.add_faces(other.faces());
  geom_input.add_edges(other.edges());
  this->fill(geom_input);
  std::vector<ComponentInstance> instances = other.instances();
  for (size_t i=0; i < instances.size(); ++i) {
    this->add_instance(instances[i].definition(), instances[i].transformation(), instances[i].name());
  }
  std::vector<Group> other_groups = other.groups();
  for (size_t i=0; i < other_groups.size(); ++i) {
    Group new_group = this->add_group();
    new_group.entities().add(other_groups[i].entities());
    new_group.transformation(other_groups[i].transformation());
    new_group.name(other_groups[i].name());
  }
  // TODO: other geometry types need to be added.
}



SUResult Entities::fill(GeometryInput &geom_input) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::fill(): Entities is null");
  }
  // Check geom_input is not empty.
  if (geom_input.empty()) {
    return SU_ERROR_NONE;
  }

  // For the indexes of the GeometryInputRef to make sense after we fill the Entities object with its contents, we need to know how many of each entitity currently exists in the Entities object
  size_t num_faces_before = 0;
  SUResult res = SUEntitiesGetNumFaces(m_entities, &num_faces_before);
  assert(res == SU_ERROR_NONE);

  SUResult fill_res = SUEntitiesFill(m_entities, geom_input.m_geometry_input, true);
  assert(fill_res == SU_ERROR_NONE);

  // Now add other data that SUEntitiesFill cannot add to the entities.
  // Apply properties to Faces
  // TODO: there is an assumption that the faces added to an Entities object is added in sequence, according to the index number.  So that (num_faces_before + face_index_of_geom_input) correspond to the Face number in the Entities object. This needs to be tested.
  size_t num_faces_after = 0;
  res = SUEntitiesGetNumFaces(m_entities, &num_faces_after);
  assert(res == SU_ERROR_NONE);
  /*
  std::vector<std::pair<size_t, Face>> faces_to_add = geom_input.faces();
  // If all of the faces in the geom_input were not added, it will not be possible to find the added face by looking at its index.
  // TODO: it is clear that if two overlapping faces are put into GeometryInput object, when output into the Entities object, the result could be three or more faces.  The following code will not recognise this. This shortcoming needs to be overcome.
  if ((num_faces_after-num_faces_before) == geom_input.num_faces()) {

    SUFaceRef faces_after[num_faces_after];
    res = SUEntitiesGetFaces(m_entities, num_faces_after, &faces_after[0], &num_faces_after);
    assert(res == SU_ERROR_NONE);
    for (size_t i=0; i < faces_to_add.size(); ++i) {
      size_t new_face_index = faces_to_add[i].first;
      Face new_face(faces_after[(num_faces_before+new_face_index-1)]);
      new_face.copy_properties_from(faces_to_add[i].second);
      new_face.copy_attributes_from(faces_to_add[i].second);
      // Set attributes for the edges that bound the face.
      std::vector<Loop> loops = faces_to_add[i].second.loops();
      std::vector<Loop> new_loops = new_face.loops();

      for (size_t j=0; j < loops.size(); ++j) {
        std::vector<Edge> old_edges = loops[j].edges();
        std::vector<Edge> new_edges = new_loops[j].edges();
        // If there are more new edges, then it means that some edges have been split during a merging operation.  The trick to find these is to see if the end location of the new and old edges match.
        size_t new_edge_index = 0;
        size_t old_edge_index = 0;
        while( old_edge_index < old_edges.size()) {
          do {
            new_edges[new_edge_index].copy_properties_from(old_edges[old_edge_index]);
            // TODO: to save on a little performance, only a few of the DrawingElement properties need to be copied, as hidden and soft properties are already set from GeometryInput object.
            new_edges[new_edge_index].copy_attributes_from(old_edges[old_edge_index]);
            ++new_edge_index;
          }
          while ((old_edges.size() < new_edges.size()) &&
                 (new_edge_index < new_edges.size()) &&
                 (new_edges[new_edge_index-1].end().position() != old_edges[old_edge_index+1].end().position()));
          ++old_edge_index;
        }
      }
    }
  }
  */
  return fill_res;
}

std::vector<Face> Entities::add_faces(std::vector<Face>& faces) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::add_faces(): Entities is null");
  }
  std::vector<SUFaceRef> refs(faces.size(), SU_INVALID);
  std::transform(faces.begin(), faces.end(), refs.begin(), [](const CW::Face& face) {return face.ref(); });

  SUResult res = SUEntitiesAddFaces(m_entities, refs.size(), refs.data());
  assert(res == SU_ERROR_NONE);

  // Transfer ownership of each face
  for (auto& face : faces)
    face.attached(true);

  return faces;
}

std::vector<Edge> Entities::add_edges(std::vector<Edge>& edges) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::add_edges(): Entities is null");
  }
  std::vector<SUEdgeRef> refs(edges.size(), SU_INVALID);
  std::transform(edges.begin(), edges.end(), refs.begin(), [](const CW::Edge& edge) {return edge.ref(); });

  SUResult res = SUEntitiesAddEdges(m_entities, refs.size(), refs.data());
  assert(res == SU_ERROR_NONE);

  // Transfer ownership of each edge
  for (auto& edge : edges)
      edge.attached(true);

  return edges;
}


Edge Entities::add_edge(Edge& edge) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::add_edge(): Entities is null");
  }
  SUEdgeRef edge_ref = edge.ref();
  SUResult res = SUEntitiesAddEdges(m_entities, 1, &edge_ref);
  assert(res == SU_ERROR_NONE);
  edge.attached(true);
  return edge;
}


void Entities::add_instance(ComponentInstance& instance) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::add_instance(): Entities is null");
  }
  if (!instance) {
    throw std::invalid_argument("CW::Entities::add_instance(): ComponentInstance argument is invalid");
  }
  SUResult res = SUEntitiesAddInstance(m_entities, instance, nullptr);
  assert(res == SU_ERROR_NONE);
  instance.attached(true);
}


ComponentInstance Entities::add_instance(const ComponentDefinition& definition, const Transformation& transformation, const String& name){
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::add_instance(): Entities is null");
  }
  if (!definition) {
    throw std::invalid_argument("CW::Entities::add_instance(): ComponentDefinition argument is invalid");
  }
  SUComponentInstanceRef instance = SU_INVALID;
  SUResult res = SUComponentDefinitionCreateInstance(definition.ref(), &instance);
  assert(res == SU_ERROR_NONE);
  SUTransformation transform = transformation.ref();
  res = SUComponentInstanceSetTransform(instance, &transform);
  assert(res == SU_ERROR_NONE);
  if (name.empty()) {
    res = SUEntitiesAddInstance(m_entities, instance, NULL);
  }
  else {
    SUStringRef name_ref = name.ref();
    res = SUEntitiesAddInstance(m_entities, instance, &name_ref);
  }
  assert(res == SU_ERROR_NONE);
  return ComponentInstance(instance, true);
}


// TODO: add_group needs to be refined

Group Entities::add_group(const ComponentDefinition& definition, const Transformation& transformation) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::add_group(): Entities is null");
  }
  if (!definition) {
    throw std::invalid_argument("CW::Entities::add_group(): ComponentDefinition argument is invalid");
  }
  if (!definition.is_group()) {
    throw std::invalid_argument("CW::Entities::add_group(): ComponentDefinition given is not a group");
  }
  // Groups cannot be created with a component definition and transformation objects.  Instead, the geometry must be copied in to a new Entities object in the group.
  Group new_group = this->add_group();
  Entities group_entities = new_group.entities();
  Entities def_entities = definition.entities();
  // Add geometry one by one to Geometry input.
  GeometryInput geom_input;
  std::vector<Face> def_faces = def_entities.faces();
  for (size_t i=0; i < def_faces.size(); ++i) {
    geom_input.add_face(def_faces[i]);
  }
  std::vector<Edge> def_edges = def_entities.edges(true);
  for (size_t i=0; i < def_edges.size(); ++i) {
    group_entities.add_edge(def_edges[i]);
  }
  group_entities.fill(geom_input);
  // Also add instances and groups
  std::vector<Group> def_groups = def_entities.groups();
  for (size_t i=0; i < def_groups.size(); ++i) {
    group_entities.add_group(def_groups[i].definition() , def_groups[i].transformation());
  }
  std::vector<ComponentInstance> def_instances = def_entities.instances();
  for (size_t i=0; i < def_instances.size(); ++i) {
    group_entities.add_instance(def_instances[i].definition(), def_instances[i].transformation());
  }

  // TODO: add other entities to the group (construction lines, curves, etc)
  return new_group;

  // TODO: the way groups are implemented are a problem.  Come back to this.
  //SUResult res = SUComponentDefinitionCreateGroup(definition.ref(), &group);
  //assert(res == SU_ERROR_NONE);
  //SUTransformation transform = transformation.ref();
  //res = SUComponentInstanceSetTransform(instance, &transform);
}

Group Entities::add_group() {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::add_group(): Entities is null");
  }
  Group new_group;
  // Add group to the entities object before populating it.
  SUResult res = SUEntitiesAddGroup(m_entities, new_group.ref());
  assert(res == SU_ERROR_NONE);
  return new_group;
}


bool Entities::transform_entities(std::vector<Entity>& elems, const Transformation& transform) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::transform_entities(): Entities is null");
  }
  SUTransformation trans_ref = transform.ref();
  SUResult res = SUEntitiesTransform(m_entities, elems.size(), elems[0], &trans_ref);
  assert(res == SU_ERROR_NONE || res == SU_ERROR_GENERIC);
  if (res == SU_ERROR_UNSUPPORTED) {
    throw std::invalid_argument("CW::Entities::transform_entities(): One of the elements given in the Entity vector is not contained by this Entities object.");
  }
  else if (SU_ERROR_GENERIC) {
    return false;
  }
  return true;
}


bool Entities::transform_entities(std::vector<Entity>& elems, std::vector<Transformation>& transforms) {
  if (!SUIsValid(m_entities)) {
    throw std::logic_error("CW::Entities::transform_entities(): Entities is null");
  }
  if (elems.size() != transforms.size()) {
    throw std::invalid_argument("CW::Entities::transform_entities(): different number of elements to transformation objects given - the same number must be given.");
  }
  assert(elems.size() == transforms.size());
  SUResult res = SUEntitiesTransformMultiple(m_entities, elems.size(), elems[0], transforms[0]);
  if (res == SU_ERROR_UNSUPPORTED) {
    throw std::invalid_argument("CW::Entities::transform_entities(): One of the elements given in the Entity vector is not contained by this Entities object.");
  }
  else if (SU_ERROR_GENERIC) {
    return false;
  }
  return true;
}




Entities::operator SUEntitiesRef() {
  return m_entities;
}

} /* namespace CW */
