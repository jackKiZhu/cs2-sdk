#pragma once

#define MAX_SPLITSCREEN_PLAYERS 150

template <typename T, class I = int>
struct repeated_ptr_t {
    struct rep_t {
        int alloc_size;
        PAD(4);
        T* mem[];
    };

    struct iterator_t {
        iterator_t(repeated_ptr_t<T, I>* repeated_ptr, const I idx) : idx(idx), repeated_ptr(repeated_ptr) {}

        iterator_t& operator++() {
            idx = repeated_ptr->next(idx);
            return *this;
        }

        iterator_t operator++(int) {
            iterator_t copy = *this;
            ++(*this);
            return copy;
        }

        iterator_t& operator--() {
            idx = (idx == repeated_ptr->invalid_index() ? idx : repeated_ptr->prev(idx));
            return *this;
        }

        iterator_t operator--(int) {
            iterator_t copy = *this;
            --(*this);
            return copy;
        }

        bool operator==(iterator_t it) const { return idx == it.idx; }
        bool operator!=(iterator_t it) const { return idx != it.idx; }

        T& operator*() { return repeated_ptr->elem(idx); }
        T* operator->() { return (&**this); }

        I idx;
        repeated_ptr_t<T, I>* repeated_ptr;
    };

    T& operator[](const I idx) { return *mem->mem[idx]; }
    const T& operator[](const I idx) const { return *mem->mem[idx]; }

    T& elem(const I idx) { return *mem->mem[idx]; }
    const T& elem(const I idx) const { return *mem->mem[idx]; }

    static I invalid_index() { return static_cast<I>(-1); }
    bool is_valid_index(I idx) const { return idx >= 0 && idx < cur_size && mem && idx < mem->alloc_size; }

    auto first() const { return is_valid_index(0) ? 0 : invalid_index(); }
    auto next(const I idx) const { return is_valid_index(idx + 1) ? idx + 1 : invalid_index(); }
    auto prev(const I idx) const { return is_valid_index(idx - 1) ? idx - 1 : invalid_index(); }

    auto begin() { return iterator_t(this, first()); }
    auto end() { return iterator_t(this, invalid_index()); }

    int cur_size;
    int total_size;
    rep_t* mem;
};

struct message_t {
    PAD(0x18);
};

		struct cmsg_vector : message_t {
    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(this);
    }

    float x;
    float y;
    float z;
    float w;
};  // struct cmsg_vector : message_t

struct cmsg_vector2d : message_t {
    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(this);
    }

    float x;
    float y;
};  // struct cmsg_vector2d : message_t

struct cmsg_qangle : message_t {
    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(this);
    }

    float x;
    float y;
    float z;
};  // struct cmsg_qangle : message_t

struct csgointerpolation_info_pb : message_t {
    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(this);
    }

    int src_tick;
    int dst_tick;
    float frac;
};  // struct csgointerpolation_info_pb : message_t

struct csgoinput_history_entry_pb : message_t {
    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(this);
    }

    cmsg_qangle* view_angles;
    cmsg_vector* shoot_position;
    cmsg_vector* target_head_pos_check;
    cmsg_vector* target_abs_pos_check;
    cmsg_qangle* target_abs_ang_check;
    csgointerpolation_info_pb* cl_interp;
    csgointerpolation_info_pb* sv_interp0;
    csgointerpolation_info_pb* sv_interp1;
    csgointerpolation_info_pb* player_interp;
    int render_tick_count;
    float render_tick_fraction;
    int player_tick_count;
    float player_tick_fraction;
    int frame_number;
    int target_ent_index;
};  // struct csgoinput_history_entry_pb : message_t

struct cbase_user_cmd_pb;

struct cuser_cmd_base_pb : message_t {
    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(this);
    }

    cbase_user_cmd_pb* base;
};  // struct cuser_cmd_base_pb : message_t

struct csgouser_cmd_pb : message_t {
    enum class eweapon_decision : uint32_t {
        k_e_none = 0x0,
        k_e_primary_attack = 0x1,
        k_e_secondary_attack = 0x2,
        k_e_zoom = 0x3,
        k_e_reload = 0x4,
        k_e_grenade_throw = 0x5,
        k_e_grenade_hold = 0x6,
        k_e_grenade_hold_then_throw = 0x7,
    };  // enum class eweapon_decision : uint32_t

    enum class egrenade_strength : uint32_t {
        k_e_low = 0x0,
        k_e_mid = 0x1,
        k_e_high = 0x2,
    };  // enum class egrenade_strength : uint32_t

    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(this);
    }

    PAD(8);
    repeated_ptr_t<csgoinput_history_entry_pb> input_history;
    cbase_user_cmd_pb* base;
    csgouser_cmd_pb::eweapon_decision* weapon_decision;
    csgouser_cmd_pb::egrenade_strength* target_grenade_strength;
    int attack1_start_history_index;
    int attack2_start_history_index;
    int attack3_start_history_index;
    int weapon_decision_weapon;
};  // struct csgouser_cmd_pb : message_t

class PBBase {
   public:
    PAD(0x18);  // 0x0 (0x18)
};

class CMsgQAngle : PBBase {
   public:
    Vector viewangles; // 0x18 (0xC)
}; // Size: 0x24

class CBaseUserCmdPB {
   public:
    PAD(0x40);  // 0x0 (0x40)
    CMsgQAngle* msgAngle;   // 0x40 (0x8)
    uint32_t commandNumber; // 0x48 (0x4)
    uint32_t tickcount; // 0x4C (0x4)
    Vector moves; // 0x50 (0xC)
    PAD(0x24); // 0x5C (0x24)
}; // Size: 0x80

class CSubTickContainer {
   public:
    uint32_t tickcount; // 0x0 (0x4)
    PAD(0x4); // 0x4 (0x4)
    void* inputHistory; // 0x8 (0x8)
}; // Size: 0x10

class CUserCmd {
   public:
    void* vftable; // 0x0 (0x8)
    uint64_t qword8; // 0x8 (0x8)
    uint32_t flags; // 0x10 (0x4)
    PAD(0xC); // 0x14 (0xC)
    CSubTickContainer subtick; // 0x20 (0x10)
    CBaseUserCmdPB* baseCmd; // 0x30 (0x8)
    PAD(0x18); // 0x38 (0x18)
    uint64_t qword50; // 0x50 (0x8)
    PAD(32); // 0x58 (0x20)
    std::byte b; // 0x78 (0x1)
    PAD(7); // 0x79 (0x7)
    int32_t dword80; // 0x80 (0x4)
    PAD(0x4); // 0x84 (0x4)
}; // Size: 0x88 

class CInButtonState {
   public:
    PAD(0x8);
    uint64_t a[3];
};

/*
class CUserCmd : csgouser_cmd_pb {
   public:
    CInButtonState buttons;
    PAD(0x30);
};*/
static_assert(sizeof(CUserCmd) == 0x88);
static_assert(offsetof(CUserCmd, baseCmd) == 0x30);

class CCSGOInput {
   public:
    static CCSGOInput* Get();

    PAD(0x250); // 0x0 (0x250)
    CUserCmd commands[MAX_SPLITSCREEN_PLAYERS]; // 0x250 (0x4FB0)
    PAD(0x1); // 0x5200 (0x1)
    bool inThirdperson; // 0x5201 (0x1)
    PAD(0x22); // 0x5202 (0x22)
    int sequenceNumber; // 0x5224 (0x4)
    int dword5428; // 0x5228 (0x4)
    PAD(20);

    // 0x5390 viewangles

    /*
     _QWORD qword5248;
  _QWORD qword5250;
  _BYTE gap5258[312];
  _QWORD qword5390;
  _DWORD dword5398;
  _DWORD dword539C;
  _BYTE gap53A0[136];
  int int5428;
  _QWORD qword5430;
    */

    uint32_t GetSequenceNumber();
    CUserCmd* GetUserCmd();
    CUserCmd* GetUserCmd(uint32_t sequenceNumber);
};
