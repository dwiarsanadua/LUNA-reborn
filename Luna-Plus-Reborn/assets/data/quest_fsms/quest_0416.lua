--[[
  Quest 416: [Quest] Illusion of Kierra
  Level Required: 80
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [415]
  Rewards: EXP=1857697, Gold=337008
]]

local fsm = require('fsm_engine')

local quest_416 = fsm:new({
    id = 416,
    name = "[Quest] Illusion of Kierra",
    level_required = 80,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {415},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 416: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 416: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 416: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 266,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 416: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 416: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 416: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 416: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 416: Rewards given")
                self:give_rewards({
                    exp = 1857697,
                    gold = 337008,
                })
            end,
            transitions = {},
        },
    },
})

return quest_416