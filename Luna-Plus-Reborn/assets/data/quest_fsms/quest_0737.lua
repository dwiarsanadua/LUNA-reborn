--[[
  Quest 737: [Quest] Union's Top Secret
  Level Required: 59
  NPC Start: 9, NPC Complete: 117
  Prerequisites: [736]
  Rewards: EXP=619384
]]

local fsm = require('fsm_engine')

local quest_737 = fsm:new({
    id = 737,
    name = "[Quest] Union's Top Secret",
    level_required = 59,
    npc_start = 9,
    npc_complete = 117,
    prerequisites = {736},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 737: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 737: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 737: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 737: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 737: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 737: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 737: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 737: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 737: Rewards given")
                self:give_rewards({
                    exp = 619384,
                    items = {
                        { item_id = 376, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_737