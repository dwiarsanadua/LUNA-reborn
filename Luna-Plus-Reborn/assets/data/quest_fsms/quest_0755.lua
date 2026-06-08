--[[
  Quest 755: [Quest] Desolate Ravine
  Level Required: 0
  NPC Start: 400, NPC Complete: 401
  Rewards: Gold=2000
]]

local fsm = require('fsm_engine')

local quest_755 = fsm:new({
    id = 755,
    name = "[Quest] Desolate Ravine",
    level_required = 0,
    npc_start = 400,
    npc_complete = 401,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 755: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 755: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 755: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 755: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 401,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 755: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 755: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 401,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 755: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 755: Rewards given")
                self:give_rewards({
                    gold = 2000,
                    items = {
                        { item_id = 12009425, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_755