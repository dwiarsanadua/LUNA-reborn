--[[
  Quest 72: Meeting with The Guild Manager
  Level Required: 4
  NPC Start: 5, NPC Complete: 6
  Prerequisites: [71]
  Rewards: EXP=50
]]

local fsm = require('fsm_engine')

local quest_72 = fsm:new({
    id = 72,
    name = "Meeting with The Guild Manager",
    level_required = 4,
    npc_start = 5,
    npc_complete = 6,
    prerequisites = {71},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 72: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 72: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 72: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 5,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 72: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 72: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 72: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 72: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 72: Rewards given")
                self:give_rewards({
                    exp = 50,
                    items = {
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_72