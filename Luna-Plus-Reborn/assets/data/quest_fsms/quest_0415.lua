--[[
  Quest 415: [Quest] Queen's mirror
  Level Required: 79
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [414]
  Rewards: EXP=2101671, Gold=185776
]]

local fsm = require('fsm_engine')

local quest_415 = fsm:new({
    id = 415,
    name = "[Quest] Queen's mirror",
    level_required = 79,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {414},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 415: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 415: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 415: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 74,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 415: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 415: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 415: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 415: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 415: Rewards given")
                self:give_rewards({
                    exp = 2101671,
                    gold = 185776,
                    items = {
                        { item_id = 304, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_415