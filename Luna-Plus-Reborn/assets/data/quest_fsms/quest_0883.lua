--[[
  Quest 883: Voices in Nera Port
  Level Required: 95
  NPC Start: 128, NPC Complete: 535
  Rewards: EXP=7022796, Gold=77721
]]

local fsm = require('fsm_engine')

local quest_883 = fsm:new({
    id = 883,
    name = "Voices in Nera Port",
    level_required = 95,
    npc_start = 128,
    npc_complete = 535,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 883: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 883: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 883: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 290,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 883: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 883: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 883: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 883: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 883: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 883: Rewards given")
                self:give_rewards({
                    exp = 7022796,
                    gold = 77721,
                })
            end,
            transitions = {},
        },
    },
})

return quest_883