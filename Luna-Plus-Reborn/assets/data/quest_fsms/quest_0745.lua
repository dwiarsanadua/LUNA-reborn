--[[
  Quest 745: [Quest] Dragon?
  Level Required: 67
  NPC Start: 126, NPC Complete: 126
  Prerequisites: [744]
  Rewards: EXP=1210949
]]

local fsm = require('fsm_engine')

local quest_745 = fsm:new({
    id = 745,
    name = "[Quest] Dragon?",
    level_required = 67,
    npc_start = 126,
    npc_complete = 126,
    prerequisites = {744},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 745: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 745: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 745: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 372,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 745: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 396,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 745: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 745: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 745: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 745: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 745: Rewards given")
                self:give_rewards({
                    exp = 1210949,
                    items = {
                        { item_id = 368, count = 1 },
                        { item_id = 369, count = 1 },
                        { item_id = 368, count = 3 },
                        { item_id = 369, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_745