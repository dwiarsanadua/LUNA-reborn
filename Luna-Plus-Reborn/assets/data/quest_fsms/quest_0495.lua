--[[
  Quest 495: [Special][Quest] Salam
  Level Required: 65
  NPC Start: 46, NPC Complete: 46
  Rewards: EXP=1419166, Gold=213180
]]

local fsm = require('fsm_engine')

local quest_495 = fsm:new({
    id = 495,
    name = "[Special][Quest] Salam",
    level_required = 65,
    npc_start = 46,
    npc_complete = 46,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 495: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 495: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 495: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 372,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 495: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 495: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 495: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 46,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 495: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 495: Rewards given")
                self:give_rewards({
                    exp = 1419166,
                    gold = 213180,
                    items = {
                        { item_id = 335, count = 1 },
                        { item_id = 335, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_495