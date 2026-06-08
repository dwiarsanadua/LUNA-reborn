--[[
  Quest 928: [Quest] Black Hair Smiling
  Level Required: 102
  NPC Start: 535, NPC Complete: 536
  Rewards: EXP=9037116, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_928 = fsm:new({
    id = 928,
    name = "[Quest] Black Hair Smiling",
    level_required = 102,
    npc_start = 535,
    npc_complete = 536,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 928: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 928: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 928: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 298,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 928: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 928: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 928: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 928: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 536,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 928: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 928: Rewards given")
                self:give_rewards({
                    exp = 9037116,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_928