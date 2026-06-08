--[[
  Quest 382: [A Class] Troll Destruction
  Level Required: 70
  NPC Start: 95, NPC Complete: 95
  Prerequisites: [380]
  Rewards: EXP=1264849
]]

local fsm = require('fsm_engine')

local quest_382 = fsm:new({
    id = 382,
    name = "[A Class] Troll Destruction",
    level_required = 70,
    npc_start = 95,
    npc_complete = 95,
    prerequisites = {380},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 382: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 382: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 382: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 104,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 382: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 105,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 382: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 382: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 382: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 382: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 382: Rewards given")
                self:give_rewards({
                    exp = 1264849,
                    items = {
                        { item_id = 30000503, count = 1 },
                        { item_id = 30000312, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_382