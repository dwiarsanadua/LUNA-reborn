--[[
  Quest 991: Quest 991
  Level Required: 0
  NPC Start: 302, NPC Complete: 303
  Prerequisites: [990]
]]

local fsm = require('fsm_engine')

local quest_991 = fsm:new({
    id = 991,
    name = "Quest 991",
    level_required = 0,
    npc_start = 302,
    npc_complete = 303,
    prerequisites = {990},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 991: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 302,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 991: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 991: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 302,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 991: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 303,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 991: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 991: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 303,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 991: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 991: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000612, count = 1 },
                        { item_id = 11007753, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_991