--[[
  Quest 953: Surprise Bite 2
  Level Required: 0
  NPC Start: 542, NPC Complete: 542
  Prerequisites: [952]
  Rewards: EXP=15805326, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_953 = fsm:new({
    id = 953,
    name = "Surprise Bite 2",
    level_required = 0,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {952},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 953: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 953: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 953: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 120,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 953: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 953: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 953: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 953: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 953: Rewards given")
                self:give_rewards({
                    exp = 15805326,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_953