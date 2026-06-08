--[[
  Quest 402: [Quest] Eternal despair
  Level Required: 61
  NPC Start: 90, NPC Complete: 90
  Prerequisites: [401]
  Rewards: EXP=549177, Gold=24000
]]

local fsm = require('fsm_engine')

local quest_402 = fsm:new({
    id = 402,
    name = "[Quest] Eternal despair",
    level_required = 61,
    npc_start = 90,
    npc_complete = 90,
    prerequisites = {401},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 402: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 402: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 402: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 101,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 402: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 402: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 402: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 402: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 402: Rewards given")
                self:give_rewards({
                    exp = 549177,
                    gold = 24000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_402