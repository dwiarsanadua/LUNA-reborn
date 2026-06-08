--[[
  Quest 425: [Party][Quest] The first Qualifying exam.
  Level Required: 82
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [424]
  Rewards: EXP=2484438
]]

local fsm = require('fsm_engine')

local quest_425 = fsm:new({
    id = 425,
    name = "[Party][Quest] The first Qualifying exam.",
    level_required = 82,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {424},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 425: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 425: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 425: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 274,
                    count = 70,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 425: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 425: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 425: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 425: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 425: Rewards given")
                self:give_rewards({
                    exp = 2484438,
                    items = {
                        { item_id = 30000076, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_425