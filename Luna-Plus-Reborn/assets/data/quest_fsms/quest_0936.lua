--[[
  Quest 936: Attractive Hair 2
  Level Required: 104
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [935]
  Rewards: EXP=13661759
]]

local fsm = require('fsm_engine')

local quest_936 = fsm:new({
    id = 936,
    name = "Attractive Hair 2",
    level_required = 104,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {935},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 936: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 936: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 936: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 329,
                    count = 120,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 936: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 936: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 936: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 936: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 936: Rewards given")
                self:give_rewards({
                    exp = 13661759,
                })
            end,
            transitions = {},
        },
    },
})

return quest_936