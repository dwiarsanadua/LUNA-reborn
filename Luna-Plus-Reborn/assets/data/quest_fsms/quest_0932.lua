--[[
  Quest 932: When Good
  Level Required: 103
  NPC Start: 535, NPC Complete: 537
  Rewards: EXP=9884005, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_932 = fsm:new({
    id = 932,
    name = "When Good",
    level_required = 103,
    npc_start = 535,
    npc_complete = 537,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 932: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 932: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 932: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 297,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 932: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 932: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 932: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 932: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 932: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 932: Rewards given")
                self:give_rewards({
                    exp = 9884005,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_932