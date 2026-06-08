--[[
  Quest 592: [Quest] Red Turtle Population Control
  Level Required: 0
  NPC Start: 301, NPC Complete: 301
  Prerequisites: [591]
]]

local fsm = require('fsm_engine')

local quest_592 = fsm:new({
    id = 592,
    name = "[Quest] Red Turtle Population Control",
    level_required = 0,
    npc_start = 301,
    npc_complete = 301,
    prerequisites = {591},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 592: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 592: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 592: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 7,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 592: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 592: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 592: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 592: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 592: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001101, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_592