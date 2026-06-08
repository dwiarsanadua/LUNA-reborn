--[[
  Quest 240: [Repeatable] Eye See a Giant Problem
  Level Required: 27
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [239]
  Rewards: EXP=19420, Gold=3500
]]

local fsm = require('fsm_engine')

local quest_240 = fsm:new({
    id = 240,
    name = "[Repeatable] Eye See a Giant Problem",
    level_required = 27,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {239},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 240: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 240: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 240: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 54,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 240: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 240: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 240: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 240: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 240: Rewards given")
                self:give_rewards({
                    exp = 19420,
                    gold = 3500,
                    items = {
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_240