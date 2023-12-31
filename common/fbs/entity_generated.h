// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_ENTITY_ENTITYBUFFER_H_
#define FLATBUFFERS_GENERATED_ENTITY_ENTITYBUFFER_H_
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "flatbuffers/flatbuffers.h"

namespace EntityBuffer {

struct Vec2;

struct Transform;
struct TransformBuilder;

struct Dynamic;
struct DynamicBuilder;

struct KeyState;
struct KeyStateBuilder;

struct Player;
struct PlayerBuilder;

struct Outline;
struct OutlineBuilder;

struct Animation;
struct AnimationBuilder;

struct Entity;
struct EntityBuilder;

enum BodyType {
  BodyType_Mob = 0,
  BodyType_Transparent = 1,
  BodyType_Ledge = 2,
  BodyType_Solid = 3,
  BodyType_MIN = BodyType_Mob,
  BodyType_MAX = BodyType_Solid
};

inline const BodyType (&EnumValuesBodyType())[4] {
  static const BodyType values[] = {
    BodyType_Mob,
    BodyType_Transparent,
    BodyType_Ledge,
    BodyType_Solid
  };
  return values;
}

inline const char * const *EnumNamesBodyType() {
  static const char * const names[5] = {
    "Mob",
    "Transparent",
    "Ledge",
    "Solid",
    nullptr
  };
  return names;
}

inline const char *EnumNameBodyType(BodyType e) {
  if (flatbuffers::IsOutRange(e, BodyType_Mob, BodyType_Solid)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesBodyType()[index];
}

enum Color {
  Color_Yellow = 0,
  Color_Red = 1,
  Color_Green = 2,
  Color_Blue = 3,
  Color_Black = 4,
  Color_White = 5,
  Color_MIN = Color_Yellow,
  Color_MAX = Color_White
};

inline const Color (&EnumValuesColor())[6] {
  static const Color values[] = {
    Color_Yellow,
    Color_Red,
    Color_Green,
    Color_Blue,
    Color_Black,
    Color_White
  };
  return values;
}

inline const char * const *EnumNamesColor() {
  static const char * const names[7] = {
    "Yellow",
    "Red",
    "Green",
    "Blue",
    "Black",
    "White",
    nullptr
  };
  return names;
}

inline const char *EnumNameColor(Color e) {
  if (flatbuffers::IsOutRange(e, Color_Yellow, Color_White)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesColor()[index];
}

enum Component {
  Component_NONE = 0,
  Component_Transform = 1,
  Component_Dynamic = 2,
  Component_KeyState = 3,
  Component_Player = 4,
  Component_Outline = 5,
  Component_Animation = 6,
  Component_MIN = Component_NONE,
  Component_MAX = Component_Animation
};

inline const Component (&EnumValuesComponent())[7] {
  static const Component values[] = {
    Component_NONE,
    Component_Transform,
    Component_Dynamic,
    Component_KeyState,
    Component_Player,
    Component_Outline,
    Component_Animation
  };
  return values;
}

inline const char * const *EnumNamesComponent() {
  static const char * const names[8] = {
    "NONE",
    "Transform",
    "Dynamic",
    "KeyState",
    "Player",
    "Outline",
    "Animation",
    nullptr
  };
  return names;
}

inline const char *EnumNameComponent(Component e) {
  if (flatbuffers::IsOutRange(e, Component_NONE, Component_Animation)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesComponent()[index];
}

template<typename T> struct ComponentTraits {
  static const Component enum_value = Component_NONE;
};

template<> struct ComponentTraits<EntityBuffer::Transform> {
  static const Component enum_value = Component_Transform;
};

template<> struct ComponentTraits<EntityBuffer::Dynamic> {
  static const Component enum_value = Component_Dynamic;
};

template<> struct ComponentTraits<EntityBuffer::KeyState> {
  static const Component enum_value = Component_KeyState;
};

template<> struct ComponentTraits<EntityBuffer::Player> {
  static const Component enum_value = Component_Player;
};

template<> struct ComponentTraits<EntityBuffer::Outline> {
  static const Component enum_value = Component_Outline;
};

template<> struct ComponentTraits<EntityBuffer::Animation> {
  static const Component enum_value = Component_Animation;
};

bool VerifyComponent(flatbuffers::Verifier &verifier, const void *obj, Component type);
bool VerifyComponentVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Vec2 FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;

 public:
  Vec2() {
    memset(static_cast<void *>(this), 0, sizeof(Vec2));
  }
  Vec2(float _x, float _y)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
};
FLATBUFFERS_STRUCT_END(Vec2, 8);

struct Transform FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef TransformBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_X = 4,
    VT_Y = 6,
    VT_WIDTH = 8,
    VT_HEIGHT = 10
  };
  int32_t x() const {
    return GetField<int32_t>(VT_X, 0);
  }
  int32_t y() const {
    return GetField<int32_t>(VT_Y, 0);
  }
  int32_t width() const {
    return GetField<int32_t>(VT_WIDTH, 0);
  }
  int32_t height() const {
    return GetField<int32_t>(VT_HEIGHT, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_X) &&
           VerifyField<int32_t>(verifier, VT_Y) &&
           VerifyField<int32_t>(verifier, VT_WIDTH) &&
           VerifyField<int32_t>(verifier, VT_HEIGHT) &&
           verifier.EndTable();
  }
};

struct TransformBuilder {
  typedef Transform Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_x(int32_t x) {
    fbb_.AddElement<int32_t>(Transform::VT_X, x, 0);
  }
  void add_y(int32_t y) {
    fbb_.AddElement<int32_t>(Transform::VT_Y, y, 0);
  }
  void add_width(int32_t width) {
    fbb_.AddElement<int32_t>(Transform::VT_WIDTH, width, 0);
  }
  void add_height(int32_t height) {
    fbb_.AddElement<int32_t>(Transform::VT_HEIGHT, height, 0);
  }
  explicit TransformBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  TransformBuilder &operator=(const TransformBuilder &);
  flatbuffers::Offset<Transform> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Transform>(end);
    return o;
  }
};

inline flatbuffers::Offset<Transform> CreateTransform(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t x = 0,
    int32_t y = 0,
    int32_t width = 0,
    int32_t height = 0) {
  TransformBuilder builder_(_fbb);
  builder_.add_height(height);
  builder_.add_width(width);
  builder_.add_y(y);
  builder_.add_x(x);
  return builder_.Finish();
}

struct Dynamic FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef DynamicBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_WIDTH = 4,
    VT_HEIGHT = 6,
    VT_FALLING = 8,
    VT_POS = 10,
    VT_PREV_POS = 12,
    VT_VEL = 14,
    VT_ACCEL = 16,
    VT_TYPE = 18
  };
  float width() const {
    return GetField<float>(VT_WIDTH, 0.0f);
  }
  float height() const {
    return GetField<float>(VT_HEIGHT, 0.0f);
  }
  bool falling() const {
    return GetField<uint8_t>(VT_FALLING, 0) != 0;
  }
  const EntityBuffer::Vec2 *pos() const {
    return GetStruct<const EntityBuffer::Vec2 *>(VT_POS);
  }
  const EntityBuffer::Vec2 *prev_pos() const {
    return GetStruct<const EntityBuffer::Vec2 *>(VT_PREV_POS);
  }
  const EntityBuffer::Vec2 *vel() const {
    return GetStruct<const EntityBuffer::Vec2 *>(VT_VEL);
  }
  const EntityBuffer::Vec2 *accel() const {
    return GetStruct<const EntityBuffer::Vec2 *>(VT_ACCEL);
  }
  EntityBuffer::BodyType type() const {
    return static_cast<EntityBuffer::BodyType>(GetField<int8_t>(VT_TYPE, 0));
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<float>(verifier, VT_WIDTH) &&
           VerifyField<float>(verifier, VT_HEIGHT) &&
           VerifyField<uint8_t>(verifier, VT_FALLING) &&
           VerifyField<EntityBuffer::Vec2>(verifier, VT_POS) &&
           VerifyField<EntityBuffer::Vec2>(verifier, VT_PREV_POS) &&
           VerifyField<EntityBuffer::Vec2>(verifier, VT_VEL) &&
           VerifyField<EntityBuffer::Vec2>(verifier, VT_ACCEL) &&
           VerifyField<int8_t>(verifier, VT_TYPE) &&
           verifier.EndTable();
  }
};

struct DynamicBuilder {
  typedef Dynamic Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_width(float width) {
    fbb_.AddElement<float>(Dynamic::VT_WIDTH, width, 0.0f);
  }
  void add_height(float height) {
    fbb_.AddElement<float>(Dynamic::VT_HEIGHT, height, 0.0f);
  }
  void add_falling(bool falling) {
    fbb_.AddElement<uint8_t>(Dynamic::VT_FALLING, static_cast<uint8_t>(falling), 0);
  }
  void add_pos(const EntityBuffer::Vec2 *pos) {
    fbb_.AddStruct(Dynamic::VT_POS, pos);
  }
  void add_prev_pos(const EntityBuffer::Vec2 *prev_pos) {
    fbb_.AddStruct(Dynamic::VT_PREV_POS, prev_pos);
  }
  void add_vel(const EntityBuffer::Vec2 *vel) {
    fbb_.AddStruct(Dynamic::VT_VEL, vel);
  }
  void add_accel(const EntityBuffer::Vec2 *accel) {
    fbb_.AddStruct(Dynamic::VT_ACCEL, accel);
  }
  void add_type(EntityBuffer::BodyType type) {
    fbb_.AddElement<int8_t>(Dynamic::VT_TYPE, static_cast<int8_t>(type), 0);
  }
  explicit DynamicBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  DynamicBuilder &operator=(const DynamicBuilder &);
  flatbuffers::Offset<Dynamic> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Dynamic>(end);
    return o;
  }
};

inline flatbuffers::Offset<Dynamic> CreateDynamic(
    flatbuffers::FlatBufferBuilder &_fbb,
    float width = 0.0f,
    float height = 0.0f,
    bool falling = false,
    const EntityBuffer::Vec2 *pos = 0,
    const EntityBuffer::Vec2 *prev_pos = 0,
    const EntityBuffer::Vec2 *vel = 0,
    const EntityBuffer::Vec2 *accel = 0,
    EntityBuffer::BodyType type = EntityBuffer::BodyType_Mob) {
  DynamicBuilder builder_(_fbb);
  builder_.add_accel(accel);
  builder_.add_vel(vel);
  builder_.add_prev_pos(prev_pos);
  builder_.add_pos(pos);
  builder_.add_height(height);
  builder_.add_width(width);
  builder_.add_type(type);
  builder_.add_falling(falling);
  return builder_.Finish();
}

struct KeyState FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef KeyStateBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_BITSET = 4
  };
  uint64_t bitset() const {
    return GetField<uint64_t>(VT_BITSET, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint64_t>(verifier, VT_BITSET) &&
           verifier.EndTable();
  }
};

struct KeyStateBuilder {
  typedef KeyState Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_bitset(uint64_t bitset) {
    fbb_.AddElement<uint64_t>(KeyState::VT_BITSET, bitset, 0);
  }
  explicit KeyStateBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  KeyStateBuilder &operator=(const KeyStateBuilder &);
  flatbuffers::Offset<KeyState> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<KeyState>(end);
    return o;
  }
};

inline flatbuffers::Offset<KeyState> CreateKeyState(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint64_t bitset = 0) {
  KeyStateBuilder builder_(_fbb);
  builder_.add_bitset(bitset);
  return builder_.Finish();
}

struct Player FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef PlayerBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_USERNAME = 6
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const flatbuffers::String *username() const {
    return GetPointer<const flatbuffers::String *>(VT_USERNAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_USERNAME) &&
           verifier.VerifyString(username()) &&
           verifier.EndTable();
  }
};

struct PlayerBuilder {
  typedef Player Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(Player::VT_ID, id, 0);
  }
  void add_username(flatbuffers::Offset<flatbuffers::String> username) {
    fbb_.AddOffset(Player::VT_USERNAME, username);
  }
  explicit PlayerBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  PlayerBuilder &operator=(const PlayerBuilder &);
  flatbuffers::Offset<Player> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Player>(end);
    return o;
  }
};

inline flatbuffers::Offset<Player> CreatePlayer(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    flatbuffers::Offset<flatbuffers::String> username = 0) {
  PlayerBuilder builder_(_fbb);
  builder_.add_username(username);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<Player> CreatePlayerDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    const char *username = nullptr) {
  auto username__ = username ? _fbb.CreateString(username) : 0;
  return EntityBuffer::CreatePlayer(
      _fbb,
      id,
      username__);
}

struct Outline FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef OutlineBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_COLOR = 4,
    VT_WIDTH = 6
  };
  EntityBuffer::Color color() const {
    return static_cast<EntityBuffer::Color>(GetField<int8_t>(VT_COLOR, 0));
  }
  int32_t width() const {
    return GetField<int32_t>(VT_WIDTH, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_COLOR) &&
           VerifyField<int32_t>(verifier, VT_WIDTH) &&
           verifier.EndTable();
  }
};

struct OutlineBuilder {
  typedef Outline Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_color(EntityBuffer::Color color) {
    fbb_.AddElement<int8_t>(Outline::VT_COLOR, static_cast<int8_t>(color), 0);
  }
  void add_width(int32_t width) {
    fbb_.AddElement<int32_t>(Outline::VT_WIDTH, width, 0);
  }
  explicit OutlineBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  OutlineBuilder &operator=(const OutlineBuilder &);
  flatbuffers::Offset<Outline> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Outline>(end);
    return o;
  }
};

inline flatbuffers::Offset<Outline> CreateOutline(
    flatbuffers::FlatBufferBuilder &_fbb,
    EntityBuffer::Color color = EntityBuffer::Color_Yellow,
    int32_t width = 0) {
  OutlineBuilder builder_(_fbb);
  builder_.add_width(width);
  builder_.add_color(color);
  return builder_.Finish();
}

struct Animation FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef AnimationBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NAMES = 4,
    VT_DIRECTION = 6
  };
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *names() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_NAMES);
  }
  bool direction() const {
    return GetField<uint8_t>(VT_DIRECTION, 0) != 0;
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NAMES) &&
           verifier.VerifyVector(names()) &&
           verifier.VerifyVectorOfStrings(names()) &&
           VerifyField<uint8_t>(verifier, VT_DIRECTION) &&
           verifier.EndTable();
  }
};

struct AnimationBuilder {
  typedef Animation Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_names(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> names) {
    fbb_.AddOffset(Animation::VT_NAMES, names);
  }
  void add_direction(bool direction) {
    fbb_.AddElement<uint8_t>(Animation::VT_DIRECTION, static_cast<uint8_t>(direction), 0);
  }
  explicit AnimationBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  AnimationBuilder &operator=(const AnimationBuilder &);
  flatbuffers::Offset<Animation> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Animation>(end);
    return o;
  }
};

inline flatbuffers::Offset<Animation> CreateAnimation(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> names = 0,
    bool direction = false) {
  AnimationBuilder builder_(_fbb);
  builder_.add_names(names);
  builder_.add_direction(direction);
  return builder_.Finish();
}

inline flatbuffers::Offset<Animation> CreateAnimationDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *names = nullptr,
    bool direction = false) {
  auto names__ = names ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*names) : 0;
  return EntityBuffer::CreateAnimation(
      _fbb,
      names__,
      direction);
}

struct Entity FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef EntityBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4,
    VT_COMPONENT_TYPE = 6,
    VT_COMPONENT = 8
  };
  int32_t id() const {
    return GetField<int32_t>(VT_ID, 0);
  }
  const flatbuffers::Vector<uint8_t> *component_type() const {
    return GetPointer<const flatbuffers::Vector<uint8_t> *>(VT_COMPONENT_TYPE);
  }
  const flatbuffers::Vector<flatbuffers::Offset<void>> *component() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<void>> *>(VT_COMPONENT);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_ID) &&
           VerifyOffset(verifier, VT_COMPONENT_TYPE) &&
           verifier.VerifyVector(component_type()) &&
           VerifyOffset(verifier, VT_COMPONENT) &&
           verifier.VerifyVector(component()) &&
           VerifyComponentVector(verifier, component(), component_type()) &&
           verifier.EndTable();
  }
};

struct EntityBuilder {
  typedef Entity Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(int32_t id) {
    fbb_.AddElement<int32_t>(Entity::VT_ID, id, 0);
  }
  void add_component_type(flatbuffers::Offset<flatbuffers::Vector<uint8_t>> component_type) {
    fbb_.AddOffset(Entity::VT_COMPONENT_TYPE, component_type);
  }
  void add_component(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<void>>> component) {
    fbb_.AddOffset(Entity::VT_COMPONENT, component);
  }
  explicit EntityBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  EntityBuilder &operator=(const EntityBuilder &);
  flatbuffers::Offset<Entity> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Entity>(end);
    return o;
  }
};

inline flatbuffers::Offset<Entity> CreateEntity(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    flatbuffers::Offset<flatbuffers::Vector<uint8_t>> component_type = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<void>>> component = 0) {
  EntityBuilder builder_(_fbb);
  builder_.add_component(component);
  builder_.add_component_type(component_type);
  builder_.add_id(id);
  return builder_.Finish();
}

inline flatbuffers::Offset<Entity> CreateEntityDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t id = 0,
    const std::vector<uint8_t> *component_type = nullptr,
    const std::vector<flatbuffers::Offset<void>> *component = nullptr) {
  auto component_type__ = component_type ? _fbb.CreateVector<uint8_t>(*component_type) : 0;
  auto component__ = component ? _fbb.CreateVector<flatbuffers::Offset<void>>(*component) : 0;
  return EntityBuffer::CreateEntity(
      _fbb,
      id,
      component_type__,
      component__);
}

inline bool VerifyComponent(flatbuffers::Verifier &verifier, const void *obj, Component type) {
  switch (type) {
    case Component_NONE: {
      return true;
    }
    case Component_Transform: {
      auto ptr = reinterpret_cast<const EntityBuffer::Transform *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Component_Dynamic: {
      auto ptr = reinterpret_cast<const EntityBuffer::Dynamic *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Component_KeyState: {
      auto ptr = reinterpret_cast<const EntityBuffer::KeyState *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Component_Player: {
      auto ptr = reinterpret_cast<const EntityBuffer::Player *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Component_Outline: {
      auto ptr = reinterpret_cast<const EntityBuffer::Outline *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case Component_Animation: {
      auto ptr = reinterpret_cast<const EntityBuffer::Animation *>(obj);
      return verifier.VerifyTable(ptr);
    }
    default: return true;
  }
}

inline bool VerifyComponentVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types) {
  if (!values || !types) return !values && !types;
  if (values->size() != types->size()) return false;
  for (flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
    if (!VerifyComponent(
        verifier,  values->Get(i), types->GetEnum<Component>(i))) {
      return false;
    }
  }
  return true;
}

inline const EntityBuffer::Entity *GetEntity(const void *buf) {
  return flatbuffers::GetRoot<EntityBuffer::Entity>(buf);
}

inline const EntityBuffer::Entity *GetSizePrefixedEntity(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<EntityBuffer::Entity>(buf);
}

inline bool VerifyEntityBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<EntityBuffer::Entity>(nullptr);
}

inline bool VerifySizePrefixedEntityBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<EntityBuffer::Entity>(nullptr);
}

inline void FinishEntityBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<EntityBuffer::Entity> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedEntityBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<EntityBuffer::Entity> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace EntityBuffer

#endif  // FLATBUFFERS_GENERATED_ENTITY_ENTITYBUFFER_H_
