--[[
  Quest 206: [Revenge] Bearly a Fair Fight
  Level Required: 15
  NPC Start: 0, NPC Complete: 0
  Prerequisites: [205]
  Rewards: EXP=7025, Gold=1500
]]

local fsm = require('fsm_engine')

local quest_206 = fsm:new({
    id = 206,
    name = "[Revenge] Bearly a Fair Fight",
    level_required = 15,
    npc_start = 0,
    npc_complete = 0,
    prerequisites = {205},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 206: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "accept",
                    target = "IN_PROGRESS",
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 206: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 67,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 206: Kill objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 206: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 0,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 206: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 206: Rewards given")
                self:give_rewards({
                    exp = 7025,
                    gold = 1500,
                    items = {
                        { item_id = 30000825, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_206