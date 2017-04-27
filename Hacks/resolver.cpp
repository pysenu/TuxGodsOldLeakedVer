#include "resolver.h"
#include "../settings.h"
#include "../Utils/netvarmanager.h"

float Settings::Resolver::ticks = 2;
float Settings::Resolver::modulo = 2;
ResolverMode Settings::Resolver::mode = ResolverMode::OFF;

void CResolver::Resolve(C_BasePlayer* ent) {
    auto cur = m_arrInfos.at(ent->GetIndex()).m_sRecords;

    ResolverMode mode = Settings::Resolver::mode;

    switch (mode) {
        case ResolverMode::FORCE: {
            ent->GetEyeAngles()->y = cur.front().m_flLowerBodyYawTarget;
            break;
        }
        case ResolverMode::DELTA: {
            ent->GetEyeAngles()->y = LBYDelta(cur.front());
            break;
        }
        case ResolverMode::STEADY: {
            if (cur.size() <= 1)
                return;
            float tickdif = static_cast<float>(cur.front().tickcount - cur.at(1).tickcount);
            float lbydif = GetDelta(cur.front().m_flLowerBodyYawTarget, cur.at(1).m_flLowerBodyYawTarget);
            float ntickdif = static_cast<float>(globalVars->tickcount - cur.front().tickcount);
            ent->GetEyeAngles()->y = (lbydif / tickdif) * ntickdif;
            break;
        }
        case ResolverMode::TICKMODULO: {
            ent->GetEyeAngles()->y = ent->GetEyeAngles()->y - GetDeltaByComparingTicks(cur);
            break;
        }
        case ResolverMode::POSEPARAM: {
            if (IsEntityMoving(ent)) {
                ent->GetEyeAngles()->y = *ent->GetLowerBodyYawTarget();
            }
            else {
                std::array<float, 24>* pose = ((C_BaseAnimating*)ent)->GetPoseParam();
                ent->GetEyeAngles()->y = pose->at(2) * 360.f - pose->at(11) * 360.f;
            }
        }
        case ResolverMode::ALL: {
            if (IsEntityMoving(ent))
                ent->GetEyeAngles()->y = *ent->GetLowerBodyYawTarget();
            else if (HasStaticRealAngle(cur))
                ent->GetEyeAngles()->y =
                        (cur.front().m_flLowerBodyYawTarget) + (RandomFloat(0.f, 1.f) > 0.5f ? 10 : -10);
            else if (HasStaticYawDifference(cur))
                ent->GetEyeAngles()->y =
                        ent->GetEyeAngles()->y - (cur.front().m_angEyeAngles.y - cur.front().m_flLowerBodyYawTarget);
            else if (HasSteadyDifference(cur)) {
                float tickdif = static_cast<float>(cur.front().tickcount - cur.at(1).tickcount);
                float lbydif = GetDelta(cur.front().m_flLowerBodyYawTarget, cur.at(1).m_flLowerBodyYawTarget);
                float ntickdif = static_cast<float>(globalVars->tickcount - cur.front().tickcount);
                ent->GetEyeAngles()->y = (lbydif / tickdif) * ntickdif;
            } else if (DeltaKeepsChanging(cur))
                ent->GetEyeAngles()->y = ent->GetEyeAngles()->y - GetDeltaByComparingTicks(cur);
            else if (LBYKeepsChanging(cur))
                ent->GetEyeAngles()->y = GetLBYByComparingTicks(cur);
            else
                ent->GetEyeAngles()->y = ent->GetEyeAngles()->y + 180;
            break;
        }
    }
}


bool& CResolver::LowerBodyYawChanged(C_BasePlayer* ent) {
    return m_arrInfos.at(ent->GetIndex()).m_bLowerBodyYawChanged;
}

void CResolver::StoreVars(C_BasePlayer* ent) {
    if (m_arrInfos.at(ent->GetIndex()).m_sRecords.size() >= Settings::Resolver::ticks) {
        m_arrInfos.at(ent->GetIndex()).m_sRecords.pop_back();
    }
    m_arrInfos.at(ent->GetIndex()).m_sRecords.push_front(CTickRecord(ent));
}

void CResolver::StoreVars(C_BasePlayer* ent, QAngle ang, float lby, float simtime, float tick) {
    if (m_arrInfos.at(ent->GetIndex()).m_sRecords.size() >= Settings::Resolver::ticks)
        m_arrInfos.at(ent->GetIndex()).m_sRecords.pop_back();
    m_arrInfos.at(ent->GetIndex()).m_sRecords.push_front(CTickRecord(ent));
}

bool& CResolver::BacktrackThisTick(C_BasePlayer* ent) {
    return m_arrInfos.at(ent->GetIndex()).m_bBacktrackThisTick;
}

CTickRecord CResolver::GetShotRecord(C_BasePlayer* ent) {
    for (auto cur : m_arrInfos[ent->GetIndex()].m_sRecords) {
        if (cur.validtick)
            return CTickRecord(cur);
    }
    return CTickRecord();
}

bool CResolver::HasStaticRealAngle(const std::deque<CTickRecord>& l, float tolerance) {
    auto minmax = std::minmax_element(std::begin(l), std::end(l), [](const CTickRecord& t1, const CTickRecord& t2) { return t1.m_flLowerBodyYawTarget < t2.m_flLowerBodyYawTarget; });
    return (fabs(minmax.first->m_flLowerBodyYawTarget - minmax.second->m_flLowerBodyYawTarget) <= tolerance);
}

bool CResolver::HasStaticRealAngle(int index, float tolerance) {
    return HasStaticRealAngle(m_arrInfos[index].m_sRecords, tolerance);
}

bool CResolver::HasStaticYawDifference(const std::deque<CTickRecord>& l, float tolerance) {
    for (auto i = l.begin(); i < l.end() - 1;) {
        if (GetDelta(LBYDelta(*i), LBYDelta(*++i)) > tolerance)
            return false;
    }
    return true;
}

bool CResolver::HasSteadyDifference(const std::deque<CTickRecord>& l, float tolerance) {
    size_t misses = 0;
    for (size_t i = 0; i < l.size() - 1; i++) {
        float tickdif = static_cast<float>(l.at(i).m_flSimulationTime - l.at(i + 1).tickcount);
        float lbydif = GetDelta(l.at(i).m_flLowerBodyYawTarget, l.at(i + 1).m_flLowerBodyYawTarget);
        float ntickdif = static_cast<float>(globalVars->tickcount - l.at(i).tickcount);
        if (((lbydif / tickdif) * ntickdif) > tolerance) misses++;
    }
    return (misses <= (l.size() / 3));
}

int CResolver::GetDifferentDeltas(const std::deque<CTickRecord>& l, float tolerance) {
    std::vector<float> vec;
    for (auto var : l) {
        float curdelta = LBYDelta(var);
        bool add = true;
        for (auto fl : vec) {
            if (!IsDifferent(curdelta, fl, tolerance))
                add = false;
        }
        if (add)
            vec.push_back(curdelta);
    }
    return vec.size();
}

int CResolver::GetDifferentLBYs(const std::deque<CTickRecord>& l, float tolerance) {
    std::vector<float> vec;
    for (auto var : l) {
        float curyaw = var.m_flLowerBodyYawTarget;
        bool add = true;
        for (auto fl : vec) {
            if (!IsDifferent(curyaw, fl, tolerance))
                add = false;
        }
        if (add)
            vec.push_back(curyaw);
    }
    return vec.size();
}

bool CResolver::DeltaKeepsChanging(const std::deque<CTickRecord>& cur, float tolerance) {
    return (GetDifferentDeltas(cur) > (int) cur.size() / 2);
}

bool CResolver::LBYKeepsChanging(const std::deque<CTickRecord>& cur, float tolerance) {
    return (GetDifferentLBYs(cur, tolerance) > (int)cur.size() / 2);
}

float CResolver::GetLBYByComparingTicks(const std::deque<CTickRecord>& l) {
    int modulo = Settings::Resolver::modulo;
    //cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 255), "\nmodulo " + modulo);
    int difangles = GetDifferentLBYs(l);
    int inc = modulo * difangles;
    for (auto var : l) {
        for (int lasttick = var.tickcount; lasttick < globalVars->tickcount; lasttick += inc) {
            if (lasttick == globalVars->tickcount)
                return var.m_flLowerBodyYawTarget;
        }
    }
    return 0.f;
}

float CResolver::GetDeltaByComparingTicks(const std::deque<CTickRecord>& l) {
    int modulo = Settings::Resolver::modulo;
    int difangles = GetDifferentDeltas(l);
    int inc = modulo * difangles;
    for (auto var : l) {
        for (int lasttick = var.tickcount; lasttick < globalVars->tickcount; lasttick += inc) {
            if (lasttick == globalVars->tickcount)
                return LBYDelta(var);
        }
    }
    return 0.f;
}

bool CResolver::IsEntityMoving(C_BasePlayer* ent) {
    return (ent->GetVelocity().Length2D() > 0.1f && ent->GetFlags() & FL_ONGROUND);
}

void CResolver::FrameStageNotify(ClientFrameStage_t stage)
{
    if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
    {
        for (int i = 1; i < engine->GetMaxClients(); ++i)
        {
            C_BasePlayer* ent = (C_BasePlayer*) entityList->GetClientEntity(i);

            if (!ent
                || ent == entityList->GetClientEntity(engine->GetLocalPlayer())
                || ent->GetDormant()
                || !ent->GetAlive()
                || ent->GetImmune()
                || ent->GetTeam() == entityList->GetClientEntity(engine->GetLocalPlayer())->GetTeam())
                continue;

            CResolver::Resolve(ent);
        }
    }
}

void CResolver::CreateMove(CUserCmd *cmd)
{
    for (int x = 1; x < engine->GetMaxClients(); ++x)
    {
        C_BasePlayer* ent = (C_BasePlayer*) entityList->GetClientEntity(x);

        if (!ent
                || ent == entityList->GetClientEntity(engine->GetLocalPlayer())
                || ent->GetDormant()
                || !ent->GetAlive()
                || ent->GetImmune()
                || ent->GetTeam() == entityList->GetClientEntity(engine->GetLocalPlayer())->GetTeam())
                continue;

        CResolver::StoreVars(ent);
        //cvar->ConsoleColorPrintf(ColorRGBA(255, 255, 255), "\nstoring variables");
    }
}