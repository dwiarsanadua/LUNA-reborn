--[[
  Quest 203: Forrest's good fight
  Level Required: 46
  NPC Start: 62, NPC Complete: 116
  Rewards: EXP=161852, Gold=5000
]]

local fsm = require('fsm_engine')

local quest_203 = fsm:new({
    id = 203,
    name = "Forrest's good fight",
    level_required = 46,
    npc_start = 62,
    npc_complete = 116,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 203: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 203: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 203: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 203: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 203: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 116,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 203: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 203: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 116,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 203: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 203: Rewards given")
                self:give_rewards({
                    exp = 161852,
                    gold = 5000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_203